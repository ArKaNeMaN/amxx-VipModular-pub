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

// Префикс ссылки на файл
stock static const __JSON_WRAPPER_REF_FILE_PREFIX[] = "File:";

static stock Json__IsWrapper(const JSON:jValue) {
    return (
        jValue != Invalid_JSON
        && json_object_has_value(jValue, __JSON_WRAPPER_FIELD_FILE, JSONString)
        && json_object_has_value(jValue, __JSON_WRAPPER_FIELD_WORKDIR, JSONString)
        && json_object_has_value(jValue, __JSON_WRAPPER_FIELD_VALUE)
        && json_get_parent(jValue) == Invalid_JSON
    );
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
    new sDir[PLATFORM_MAX_PATH];
    copy(sDir, charsmax(sDir), sFile);

    new i = 0, iLastSlash = -1;
    while (sDir[i]) {
        if (sDir[i] == '/' || sDir[i] == '\') {
            iLastSlash = i;
        }
    }

    if (iLastSlash >= 0) {
        sDir[iLastSlash] = 0; // EOS
    }

    return sDir;
}

/**
 * Парсит JSON-значение из файла, добавляя обёртку для расширения функционала.
 *
 * @param sPath     Путь до файла.
 * @param sWorkDir  Корневая директория для ссылок. По умолчанию будет директория, в которой лежит читаемый файл.
 *
 * @note            Полуечнные из этой функции хендлеры обязательно должны освобождаться при помощи Json_DeepFree.
 *                  Иначе, из-за обёртки, данные останутся в памяти.
 *
 * @return          Хендлер прочитанного из файла значения.
 */
stock JSON:Json_ParseFromFileEx(const sPath[], const sWorkDir[] = NULL_STRING) {
    new JSON:jFile = json_parse(sPath, true, true);
    
    if (jFile == Invalid_JSON) {
        log_amx("[ERROR] Can't read JSON from file '%s'.", sPath);
        return Invalid_JSON;
    }

    new JSON:jWrapper = json_init_object();
    json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_FILE, sPath);
    json_object_set_value(jWrapper, __JSON_WRAPPER_FIELD_VALUE, jFile);

    if (sWorkDir[0]) {
        json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_WORKDIR, sWorkDir);
    } else {
        json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_WORKDIR, Json__GetFileDir(sPath));
    }

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
        json_free(jValue);
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
    json_free(jParent);

    return jValue;
}

static stock JSON:Json__GetRoot(const JSON:jValue) {
    if (jValue == Invalid_JSON) {
        return Invalid_JSON;
    }

    new JSON:jParent;
    new JSON:jIterator = json_get_parent(jValue);
    while (jIterator != Invalid_JSON) {
        jParent = json_get_parent(jIterator);
        if (jParent == Invalid_JSON) {
            break;
        }
        json_free(jIterator);
        jIterator = jParent;
    }

    // А как освобождать хендлер обёртки без её удаления?)
    return jIterator;
}

static stock Json__GetFilePath(const JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0) {
    new JSON:jRoot = Json__GetRoot(jValue);

    new sPath[PLATFORM_MAX_PATH];
    if (!Json__IsWrapper(jRoot)) {
        sPath[0] = 0; // EOS
        copy(sOut, iOutLen, sPath);
        return sPath;
    }

    json_object_get_string(jRoot, __JSON_WRAPPER_FIELD_FILE, sPath, charsmax(sPath));
    copy(sOut, iOutLen, sPath);
    return sPath;
}

static stock Json__GetWorkDir(const JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0) {
    new JSON:jRoot = Json__GetRoot(jValue);

    new sPath[PLATFORM_MAX_PATH];
    if (!Json__IsWrapper(jRoot)) {
        sPath[0] = 0; // EOS
        copy(sOut, iOutLen, sPath);
        return sPath;
    }

    json_object_get_string(jRoot, __JSON_WRAPPER_FIELD_WORKDIR, sPath, charsmax(sPath));
    copy(sOut, iOutLen, sPath);
    return sPath;
}

new stock const __Json_LogForFile_templateFile[] = "[JSON][%s] File: %s";
new stock const __Json_LogForFile_templateMessage[] = "[JSON][%s] Message: %s";
stock Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...) {
    log_amx(__Json_LogForFile_templateFile, sPrefix, Json__GetFilePath(jValue));

    static sFormattedMessage[1024];
    if (numargs() >= 4) {
        vformat(sFormattedMessage, charsmax(sFormattedMessage), sMessage, 4);
        log_amx(__Json_LogForFile_templateMessage, sPrefix, sFormattedMessage);
    } else {
        log_amx(__Json_LogForFile_templateMessage, sPrefix, sMessage);
    }
}

new stock const __Json_ErrorForFile_template[] = "[JSON][ERROR] %s: %s";
stock Json_ErrorForFile(const JSON:jValue, const sMessage[], any:...) {
    static sFormattedMessage[1024];
    if (numargs() >= 4) {
        vformat(sFormattedMessage, charsmax(sFormattedMessage), sMessage, 3);
    } else {
        copy(sFormattedMessage, charsmax(sFormattedMessage), sMessage);
    }

    log_amx(__Json_ErrorForFile_template, Json__GetFilePath(jValue), sFormattedMessage);
    log_error(0, __Json_ErrorForFile_template, Json__GetFilePath(jValue), sFormattedMessage);
}

new stock const __Json_FatalErrorForFile_template[] = "[JSON] %s: %s";
stock Json_FatalErrorForFile(const JSON:jValue, const sMessage[], any:...) {
    static sFormattedMessage[1024];
    if (numargs() >= 4) {
        vformat(sFormattedMessage, charsmax(sFormattedMessage), sMessage, 3);
        set_fail_state(__Json_FatalErrorForFile_template, Json__GetFilePath(jValue), sFormattedMessage);
    } else {
        set_fail_state(__Json_FatalErrorForFile_template, Json__GetFilePath(jValue), sMessage);
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

    new Str[128];
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
stock JSON:Json_GetRefValue(&JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0) {
    new sRefFilePath[PLATFORM_MAX_PATH];
    if (!Json_IsRef(jValue, sRefFilePath, charsmax(sRefFilePath))) {
        return jValue;
    }

    new sFileName[PLATFORM_MAX_PATH];
    new sWorkDir[PLATFORM_MAX_PATH];

    if (sRefFilePath[0] == '/') {
        // Если ссылка начинается со слеша, считать её от amxmodx/configs/
        Json__GetRootConfigsPath(sWorkDir, charsmax(sWorkDir));
    } else {
        Json__GetWorkDir(jValue, sWorkDir, charsmax(sWorkDir));
        if (!sWorkDir[0]) {
            // Если у относительной ссылки нет workdir, значит ссылаться некуда)
            return jValue;
        }
    }
    
    formatex(sFileName, charsmax(sFileName), "%s/%s.json", sWorkDir, sRefFilePath);

    if (iOutLen) {
        copy(sOut, iOutLen, sFileName);
    }

    jValue = Json_ParseFromFileEx(sFileName, sFileName);
    return jValue;
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
    
    new sKey[128];
    for (new i = 0; i < json_object_get_count(jObject); i++) {
        json_object_get_name(jObject, i, sKey, charsmax(sKey));
        new JSON:jValue = json_object_get_value_at(jObject, i);

        switch (json_get_type(jValue)) {
            case JSONString: {
                new sValue[256];
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
