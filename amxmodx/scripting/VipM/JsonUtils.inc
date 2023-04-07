#if defined __UTILS_JsonUtils_included
    #endinput
#endif
#define __UTILS_JsonUtils_included

#include <amxmodx>
#include <json>

// Путь до директории, откуда начинать считать ссылки
stock static const __JSON_WRAPPER_FIELD_WORKDIR[] = "_work_dir";

// Файл, из которого было прочитано значение
stock static const __JSON_WRAPPER_FIELD_FILE[] = "_file";

// Само значение
stock static const __JSON_WRAPPER_FIELD_VALUE[] = "_value";

// Корневой хендлер обёртки (c m_bMustBeFreed == true)
stock static const __JSON_WRAPPER_FIELD_HANDLER[] = "_handler";

// Префикс ссылки на файл
stock static const __JSON_WRAPPER_REF_FILE_PREFIX[] = "File:";

#define JSON_UTILS__MAX_KEY_LENGTH 128
#define JSON_UTILS__MAX_STRING_LENGTH 256
#define JSON_UTILS__MAX_REF_LENGTH 128

static stock Json__IsWrapper(const JSON:jValue) {
    // log_amx("Json__IsWrapper 1 (jValue=%d)", jValue);
    if (
        jValue == Invalid_JSON
        || !json_is_object(jValue)
        || !json_object_has_value(jValue, __JSON_WRAPPER_FIELD_FILE, JSONString)
        || !json_object_has_value(jValue, __JSON_WRAPPER_FIELD_WORKDIR, JSONString)
        || !json_object_has_value(jValue, __JSON_WRAPPER_FIELD_HANDLER, JSONNumber)
        || !json_object_has_value(jValue, __JSON_WRAPPER_FIELD_VALUE)
    ) {
        // log_amx("Json__IsWrapper 2");
        return false;
    }

    // log_amx("Json__IsWrapper 3");
    new JSON:jParent = json_get_parent(jValue);
    if (jParent != Invalid_JSON) {
        // log_amx("Json__IsWrapper 4");
        json_free(jParent);
        return false;
    }

    // log_amx("Json__IsWrapper 5");
    return true;
}

static stock Json__GetRootConfigsPath(sOut[] = NULL_STRING, const iOutLen = 0) {
    static sPath[PLATFORM_MAX_PATH];

    if (!sPath[0]) {
        get_localinfo("amxx_configsdir", sPath, charsmax(sPath));
    }

    if (iOutLen) {
        copy(sOut, iOutLen, sPath);
    }

    return sPath;
}

static stock Json__GetFileDir(const sFile[]) {
    // log_amx("[DEBUG] Json__GetFileDir: sFile=%s", sFile);

    new sDir[PLATFORM_MAX_PATH];
    copy(sDir, charsmax(sDir), sFile);

    new i = 0, iLastSlash = -1;
    while (sDir[i]) {
        if (sDir[i] == '/' || sDir[i] == '\') {
            iLastSlash = i;
        }
        ++i;
    }

    // log_amx("[DEBUG] Json__GetFileDir: sDir=%s", sDir);
    // log_amx("[DEBUG] Json__GetFileDir: iLastSlash=%d", iLastSlash);

    if (iLastSlash >= 0) {
        sDir[max(iLastSlash - 1, 0)] = 0; // EOS
    }
    // log_amx("[DEBUG] Json__GetFileDir: return=%s", sDir);

    return sDir;
}

/**
 * Парсит JSON-значение из файла, добавляя обёртку для расширения функционала.
 *
 * @param sPath     Путь до файла.
 * @param sWorkDir  Корневая директория для ссылок. По умолчанию будет директория, в которой лежит читаемый файл.
 *
 * @note            Полученные из этой функции хендлеры обязательно должны освобождаться при помощи Json_DeepFree.
 *                  Иначе, из-за обёртки, данные останутся в памяти.
 *
 * @return          Хендлер прочитанного из файла значения.
 */
stock JSON:Json_ParseFromFileEx(const sPath[], const sWorkDir[] = NULL_STRING) {
    if (!file_exists(sPath)) {
        log_amx("[ERROR] File '%s' not found.", sPath);
        return Invalid_JSON;
    }

    new JSON:jFile = json_parse(sPath, true, true);
    // log_amx("Json_ParseFromFileEx: 1 (jFile=%d)", jFile);
    
    if (jFile == Invalid_JSON) {
        log_amx("[ERROR] Can't read JSON from file '%s'.", sPath);
        return Invalid_JSON;
    }

    // m_bMustBeFreed == true
    new JSON:jWrapper = json_init_object();
    json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_FILE, sPath);
    json_object_set_value(jWrapper, __JSON_WRAPPER_FIELD_VALUE, jFile);
    json_object_set_number(jWrapper, __JSON_WRAPPER_FIELD_HANDLER, _:jWrapper);
    // log_amx("Json_ParseFromFileEx: 2 (jWrapper=%d)", jWrapper);

    if (sWorkDir[0]) {
        json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_WORKDIR, sWorkDir);
        // log_amx("Json_ParseFromFileEx: sWorkDir=%s from param", sWorkDir);
    } else {
        json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_WORKDIR, Json__GetFileDir(sPath));
        // log_amx("Json_ParseFromFileEx: sWorkDir=%s inherit", sWorkDir);
    }

    // log_amx("Json_ParseFromFileEx: 3 (jFile=%d)", jFile);
    return jFile;
}

/**
 * Удаляет JSON-значение вместе с обёрткой.
 *
 * @param jValue    Корневой (полученный из Json_ParseFromFileEx) JSON-хендлер.
 *
 * @note            Обязательно использовать для очищения хендлеров из Json_ParseFromFileEx.
 *                  Иначе, из-за обёртки, данные останутся в памяти.
 *
 * @return          Invalid_JSON
 */
stock JSON:Json_DeepFree(&JSON:jValue) {
    new JSON:jParent;
    while (jValue != Invalid_JSON) {
        jParent = json_get_parent(jValue);
        if (jParent == Invalid_JSON) {
            Json__FreeWrapper(jValue, true);
        } else {
            json_free(jValue);
        }
        jValue = jParent;
    }
    return jValue;
}

/**
 * Удаляет JSON-значение, которое могло быть получено по ссылке.
 *
 * @param jValue    JSON-хендлер.
 *
 * @note            Если значение было получено по ссылке, полностью его удаляет из памяти.
 *                  Для обычных значений работает как json_free.
 *
 * @note            Должно использоваться для всех значений, полученных из Json_GetRefValue.
 *
 * @return          Invalid_JSON
 */
stock JSON:Json_FreeRefedValue(&JSON:jValue) {
    /*
     * Если значение получено по ссылке, его родителем будет обёртка,
     * удаление которой полностью удалит все данные.
     * 
     * А если значение было получено не по ссылке, его родителем будет обычное значение,
     * удаление которого гарантированно НЕ удалит сами данные,
     * т.к. у значения останется родитилем как минимум обёртка.
     *
     * Обёртка гарантированно будет, т.к. иначе ссылки в принципе не работают.
     */
    new JSON:jParent = json_get_parent(jValue);
    json_free(jValue);
    Json__FreeWrapper(jParent, true);

    return jValue;
}

static stock JSON:Json__FreeWrapper(&JSON:jWrapper, const bool:bFreeIfNotWrapper = false) {
    if (!Json__IsWrapper(jWrapper)) {
        Json_FreeIf(jWrapper, bFreeIfNotWrapper);
        return jWrapper;
    }

    new JSON:iRootWrapperHandler = JSON:json_object_get_number(jWrapper, __JSON_WRAPPER_FIELD_HANDLER);
    json_free(jWrapper);
    json_free(iRootWrapperHandler);

    return Invalid_JSON;
}

static stock JSON:Json__GetRoot(const JSON:jValue) {
    if (jValue == Invalid_JSON) {
        return Invalid_JSON;
    }
    // log_amx("Json__GetRoot 1 (jValue=%d)", jValue);

    new JSON:jParent;
    new JSON:jIterator = json_get_parent(jValue);
    // log_amx("Json__GetRoot 2 (jIterator=%d)", jIterator);
    if (jIterator == Invalid_JSON) {
        return jValue;
    }

    // log_amx("Json__GetRoot 3");
    while (jIterator != Invalid_JSON) {
        jParent = json_get_parent(jIterator);
        // log_amx("Json__GetRoot 4 (jParent=%d)", jParent);
        if (jParent == Invalid_JSON) {
            break;
        }
        json_free(jIterator);
        jIterator = jParent;
        // log_amx("Json__GetRoot 5 (jIterator=%d)", jIterator);
    }

    // log_amx("Json__GetRoot 6 (jIterator=%d)", jIterator);
    // m_bMustBeFreed == false
    return jIterator;
}

static stock Json__GetFilePath(const JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0, const sDefault[] = NULL_STRING) {
    // log_amx("Json__GetFilePath 1 (jValue=%d)", jValue);
    new JSON:jRoot = Json__GetRoot(jValue);
    // log_amx("Json__GetFilePath 2 (jRoot=%d)", jRoot);

    new sPath[PLATFORM_MAX_PATH];
    if (!Json__IsWrapper(jRoot)) {
        // log_amx("Json__GetFilePath 3");
        copy(sPath, charsmax(sPath), sDefault);
    } else {
        // log_amx("Json__GetFilePath 4");
        json_object_get_string(jRoot, __JSON_WRAPPER_FIELD_FILE, sPath, charsmax(sPath));
    }
    Json_FreeIf(jRoot, jRoot != jValue);

    if (iOutLen) {
        copy(sOut, iOutLen, sPath);
    }
    // log_amx("Json__GetFilePath 5 (sOut=%s)", sOut);

    return sPath;
}

static stock Json__GetWorkDir(const JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0, const sDefault[] = NULL_STRING) {
    new JSON:jRoot = Json__GetRoot(jValue);

    new sPath[PLATFORM_MAX_PATH]; 
    if (!Json__IsWrapper(jRoot)) {
        copy(sPath, charsmax(sPath), sDefault);
    } else {
        json_object_get_string(jRoot, __JSON_WRAPPER_FIELD_WORKDIR, sPath, charsmax(sPath));
    }
    Json_FreeIf(jRoot, jRoot != jValue);

    if (iOutLen) {
        copy(sOut, iOutLen, sPath);
    }

    return sPath;
}

new stock const __Json_LogForFile_templateFile[] = "[JSON][%s] File: %s";
new stock const __Json_LogForFile_templateMessage[] = "[JSON][%s] Message: %s";
stock Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...) {
    // log_amx("Json_LogForFile 1");
    log_amx(__Json_LogForFile_templateFile, sPrefix, Json__GetFilePath(jValue, .sDefault="*unknown file*"));

    static sFormattedMessage[1024];
    if (numargs() >= 4) {
        // log_amx("Json_LogForFile 2");
        vformat(sFormattedMessage, charsmax(sFormattedMessage), sMessage, 4);
        log_amx(__Json_LogForFile_templateMessage, sPrefix, sFormattedMessage);
    } else {
        // log_amx("Json_LogForFile 3");
        log_amx(__Json_LogForFile_templateMessage, sPrefix, sMessage);
    }
    // log_amx("Json_LogForFile 4");
}

new stock const __Json_ErrorForFile_template[] = "[JSON][ERROR] %s: %s";
stock Json_ErrorForFile(const JSON:jValue, const sMessage[], any:...) {
    static sFormattedMessage[1024];
    if (numargs() >= 4) {
        vformat(sFormattedMessage, charsmax(sFormattedMessage), sMessage, 3);
    } else {
        copy(sFormattedMessage, charsmax(sFormattedMessage), sMessage);
    }

    log_amx(__Json_ErrorForFile_template, Json__GetFilePath(jValue, .sDefault="*unknown file*"), sFormattedMessage);
    log_error(0, __Json_ErrorForFile_template, Json__GetFilePath(jValue, .sDefault="*unknown file*"), sFormattedMessage);
}

new stock const __Json_FatalErrorForFile_template[] = "[JSON] %s: %s";
stock Json_FatalErrorForFile(const JSON:jValue, const sMessage[], any:...) {
    static sFormattedMessage[1024];
    if (numargs() >= 4) {
        vformat(sFormattedMessage, charsmax(sFormattedMessage), sMessage, 3);
        set_fail_state(__Json_FatalErrorForFile_template, Json__GetFilePath(jValue, .sDefault="*unknown file*"), sFormattedMessage);
    } else {
        set_fail_state(__Json_FatalErrorForFile_template, Json__GetFilePath(jValue, .sDefault="*unknown file*"), sMessage);
    }
}

/**
 * Проверяет, является ли значение ссылкой.
 *
 * @param jValue    JSON-хендлер.
 * @param sFileName Буфер для записи ссылки, если значение ей является.
 * @param iLen      Размер буфера.
 *
 * @note            Не требуется при использовании Json_GetRefValue.
 *
 * @return          true, если значение является ссылкой, иначе false.
 */
stock bool:Json_IsRef(const JSON:jValue, sFileName[] = NULL_STRING, const iLen = 0) {
    if (!json_is_string(jValue)) {
        return false;
    }

    new Str[JSON_UTILS__MAX_REF_LENGTH];
    json_get_string(jValue, Str, charsmax(Str));
    if (!equali(__JSON_WRAPPER_REF_FILE_PREFIX, Str, charsmax(__JSON_WRAPPER_REF_FILE_PREFIX))) {
        return false;
    }

    if (iLen > 0) {
        copy(sFileName, iLen, Str[charsmax(__JSON_WRAPPER_REF_FILE_PREFIX)]);
    }
    
    return true;
}

/**
 * Возвращает JSON-значение с учётом ссылки.
 *
 * @param jValue    JSON-хендлер.
 * @param sOut      Буфер для записи пути до файла, если значение было ссылкой.
 * @param iOutLen   Размер буфера.
 *
 * @note            Ссылка считается от WorkDir из обёртки jValue.
 *                  Если обёртки нет, ссылка не будет обработана.
 *
 * @note            Полученное по ссылке значение также будет иметь обёртку.
 *                  WorkDir полученного значения будет наследован от jValue.
 *
 * @return          jValue, если значение не являлось ссылкой, иначе значение полученное по ссылке.
 */
stock JSON:Json_GetRefValue(&JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0, const bool:bFreeSrcIfRef = false) {
    new sRefFilePath[PLATFORM_MAX_PATH];
    if (!Json_IsRef(jValue, sRefFilePath, charsmax(sRefFilePath))) {
        return jValue;
    }

    new sFileName[PLATFORM_MAX_PATH];
    new sWorkDir[PLATFORM_MAX_PATH];

    if (sRefFilePath[0] == '/') {
        // Если ссылка начинается со слеша, считать её от amxmodx/configs/
        Json__GetRootConfigsPath(sWorkDir, charsmax(sWorkDir));
        // log_amx("[DEBUG] Json_GetRefValue: sWorkDir from root");
    } else {
        Json__GetWorkDir(jValue, sWorkDir, charsmax(sWorkDir));
        // log_amx("[DEBUG] Json_GetRefValue: sWorkDir from wrapper");
        if (!sWorkDir[0]) {
            // Если у относительной ссылки нет workdir, значит ссылаться некуда)
            return jValue;
        }
    }
    
    Json__RightTrim(sWorkDir, '/');
    formatex(sFileName, charsmax(sFileName), "%s/%s.json", sWorkDir, sRefFilePath);

    // log_amx("[DEBUG] Json_GetRefValue: sWorkDir=%s", sWorkDir);
    // log_amx("[DEBUG] Json_GetRefValue: sFileName=%s", sFileName);
    // log_amx("[DEBUG] Json_GetRefValue: sRefFilePath=%s", sRefFilePath);

    if (iOutLen) {
        copy(sOut, iOutLen, sFileName);
    }

    Json_FreeIf(jValue, bFreeSrcIfRef);
    return Json_ParseFromFileEx(sFileName, sWorkDir);
}

stock Trie:Json_ReadObjectToTrie(const JSON:jObject, &Trie:tTrie = Invalid_Trie) {
    if (tTrie == Invalid_Trie) {
        tTrie = TrieCreate();
    }
    
    if (!json_is_object(jObject)) {
        // А надо ли ошибку бросать?
        Json_ErrorForFile(jObject, "Json_ReadObjectToTrie: JSON-value must be an object.");
        return tTrie;
    }
    
    new sKey[JSON_UTILS__MAX_KEY_LENGTH];
    for (new i = 0; i < json_object_get_count(jObject); i++) {
        json_object_get_name(jObject, i, sKey, charsmax(sKey));
        new JSON:jValue = json_object_get_value_at(jObject, i);

        switch (json_get_type(jValue)) {
            case JSONString: {
                new sValue[JSON_UTILS__MAX_STRING_LENGTH];
                json_get_string(jValue, sValue, charsmax(sValue));
                TrieSetString(tTrie, sKey, sValue);
            }

            case JSONNumber:
                TrieSetCell(tTrie, sKey, json_get_number(jValue));

            case JSONBoolean:
                TrieSetCell(tTrie, sKey, json_get_bool(jValue));
        }
        json_free(jValue);
    }

    return tTrie;
}

stock bool:Json_FreeIf(&JSON:jValue, const bool:bIf) {
    if (bIf) {
        json_free(jValue);
    }
    return bIf;
}

stock bool:Json_DeepFreeIf(&JSON:jValue, const bool:bIf) {
    if (bIf) {
        Json_DeepFree(jValue);
    }
    return bIf;
}

static stock Json__RightTrim(sStr[], const cTrim = ' ') {
    new iLen = Json__StrLen(sStr);
    if (iLen <= 0) {
        return;
    }

    while (sStr[iLen - 1] == cTrim) {
        --iLen;
    }

    sStr[iLen] = 0;
}

static stock Json__StrLen(const sStr[]) {
    new iLen = 0;
    while (sStr[iLen]) {
        ++iLen;
    }
    return iLen;
}
