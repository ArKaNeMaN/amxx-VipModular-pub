#if defined __UTILS_JsonUtils_included
    #endinput
#endif
#define __UTILS_JsonUtils_included

#include <amxmodx>
#include <json>

// Метка для распознавания обёрток
stock static const __JSON_WRAPPER_MARK_KEY[] = "!@#--some-random-string-for-identify-wrapper--!@#";

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

#define Json__CompositeMacros(%1) \
    do { %1 } while(is_linux_server() == 0xDEADBEEF)

#define Json__VFormatEx(%1,%2,%3) Json__CompositeMacros( \
    if (numargs() > %3) { \
        vformat(%1, charsmax(%1), %2, %3); \
    } else { \
        copy(%1, charsmax(%1), %2); \
    } \
)

static stock Json__IsWrapper(const JSON:jValue) {
    if (jValue == Invalid_JSON || !json_is_object(jValue)) {
        return false;
    }

    if (!json_object_has_value(jValue, __JSON_WRAPPER_MARK_KEY, JSONString)) {
        return false;
    }

    new JSON:jParent = json_get_parent(jValue);
    if (jParent != Invalid_JSON) {
        json_free(jParent);
        return false;
    }

    return true;
}

static stock JSON:Json__GetWrapper(const JSON:jValue) {
    new JSON:jRoot = Json__GetRoot(jValue);

    if (jRoot == jValue) {
        return Invalid_JSON;
    }

    if (!Json__IsWrapper(jRoot)) {
        json_free(jRoot);
        
        return Invalid_JSON;
    }

    return jRoot;
}

static stock JSON:Json__FreeWrapper(&JSON:jWrapper, const bool:bFreeIfNotWrapper = false) {
    if (!Json__IsWrapper(jWrapper)) {
        Json_FreeIf(jWrapper, bFreeIfNotWrapper);
        return jWrapper;
    }

    new JSON:iRootWrapperHandler = JSON:json_object_get_number(jWrapper, __JSON_WRAPPER_FIELD_HANDLER);
    if (iRootWrapperHandler != jWrapper) {
        json_free(jWrapper);
    }
    json_free(iRootWrapperHandler);

    return Invalid_JSON;
}

static stock Json__GetWrapperPathValue(
    const JSON:jValue,
    const sKey[],
    sOut[] = NULL_STRING,
    const iOutLen = 0,
    const sDefault[] = NULL_STRING
) {
    new JSON:jWrapper = Json__GetWrapper(jValue);

    new sPath[PLATFORM_MAX_PATH];
    if (jWrapper == Invalid_JSON) {
        copy(sPath, charsmax(sPath), sDefault);
    } else {
        json_object_get_string(jWrapper, sKey, sPath, charsmax(sPath));
        json_free(jWrapper);
    }

    if (iOutLen) {
        copy(sOut, iOutLen, sPath);
    }

    return sPath;
}

static stock Json__GetFilePath(const JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0, const sDefault[] = NULL_STRING) {
    new sPath[PLATFORM_MAX_PATH];
    Json__GetWrapperPathValue(jValue, __JSON_WRAPPER_FIELD_FILE, sPath, charsmax(sPath), sDefault);
    
    copy(sOut, iOutLen, sPath);
    return sPath;
}

static stock Json__GetWorkDir(const JSON:jValue, sOut[] = NULL_STRING, const iOutLen = 0, const sDefault[] = NULL_STRING) {
    new sPath[PLATFORM_MAX_PATH];
    Json__GetWrapperPathValue(jValue, __JSON_WRAPPER_FIELD_WORKDIR, sPath, charsmax(sPath), sDefault);

    copy(sOut, iOutLen, sPath);
    return sPath;
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
        ++i;
    }

    if (iLastSlash >= 0) {
        sDir[max(iLastSlash - 1, 0)] = 0; // EOS
    }

    return sDir;
}

/**
 * Парсит JSON-значение из файла, добавляя обёртку для расширения функционала.
 *
 * @param sPath     Путь до файла.
 * @param sWorkDir  Корневая директория для ссылок. По умолчанию будет директория, в которой лежит читаемый файл.
 *
 * @note            Полученные из этой функции хендлеры обязательно должны освобождаться при помощи Json_FreeEx.
 *                  Иначе, из-за обёртки, данные останутся в памяти.
 *
 * @return          Хендлер прочитанного из файла значения.
 */
stock JSON:Json_ParseFromFileEx(const sPath[], const sWorkDir[] = NULL_STRING, const bool:bWithComments = true) {
    if (!file_exists(sPath)) {
        log_amx("[ERROR] File '%s' not found.", sPath);
        return Invalid_JSON;
    }

    new JSON:jFile = json_parse(sPath, true, bWithComments);
    if (jFile == Invalid_JSON) {
        log_amx("[ERROR] Can't read JSON from file '%s'.", sPath);
        return Invalid_JSON;
    }

    new JSON:jWrapper = json_init_object();
    json_object_set_string(jWrapper, __JSON_WRAPPER_MARK_KEY, __JSON_WRAPPER_MARK_KEY);
    json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_FILE, sPath);
    json_object_set_value(jWrapper, __JSON_WRAPPER_FIELD_VALUE, jFile);
    json_object_set_number(jWrapper, __JSON_WRAPPER_FIELD_HANDLER, _:jWrapper);

    if (sWorkDir[0]) {
        json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_WORKDIR, sWorkDir);
    } else {
        json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_WORKDIR, Json__GetFileDir(sPath));
    }

    return jFile;
}

stock JSON:Json_FreeEx(&JSON:jValue) {
    new JSON:jParent = json_get_parent(jValue);
    Json__FreeWrapper(jParent, false);
    json_free(jValue);
    
    return jValue;
}

stock JSON:Json_FreeExIf(&JSON:jValue, const bool:bIf) {
    if (bIf) {
        Json_FreeEx(jValue);
    }

    return jValue;
}

/**
 * Получение хендлера корневого JSON-значения.
 *
 * @param jValue JSON-хендлер, для которого надо найти корневое значение.
 *
 * @return JSON-хендлер найденного корневого значения.
 */
static stock JSON:Json__GetRoot(const JSON:jValue) {
    if (jValue == Invalid_JSON) {
        return Invalid_JSON;
    }

    new JSON:jParent;
    new JSON:jIterator = json_get_parent(jValue);
    if (jIterator == Invalid_JSON) {
        return jValue;
    }

    while (jIterator != Invalid_JSON) {
        jParent = json_get_parent(jIterator);
        
        if (jParent == Invalid_JSON) {
            break;
        }
        json_free(jIterator);
        jIterator = jParent;
    }

    return jIterator;
}

new stock const __Json_LogForFile_templateFile[] = "[JSON][%s] File: %s";
new stock const __Json_LogForFile_templateMessage[] = "[JSON][%s] Message: %s";
stock Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...) {
    static sFormattedMessage[1024];
    Json__VFormatEx(sFormattedMessage, sMessage, 4);
    
    log_amx(__Json_LogForFile_templateFile, sPrefix, Json__GetFilePath(jValue, .sDefault="*unknown file*"));
    log_amx(__Json_LogForFile_templateMessage, sPrefix, sFormattedMessage);
}

new stock const __Json_ErrorForFile_template[] = "[JSON][ERROR] %s: %s";
stock Json_ErrorForFile(const JSON:jValue, const sMessage[], any:...) {
    static sFormattedMessage[1024];
    Json__VFormatEx(sFormattedMessage, sMessage, 3);

    abort(0, __Json_ErrorForFile_template, Json__GetFilePath(jValue, .sDefault="*unknown file*"), sFormattedMessage);
}

new stock const __Json_FatalErrorForFile_template[] = "[JSON] %s: %s";
stock Json_FatalErrorForFile(const JSON:jValue, const sMessage[], any:...) {
    static sFormattedMessage[1024];
    Json__VFormatEx(sFormattedMessage, sMessage, 3);

    set_fail_state(__Json_FatalErrorForFile_template, Json__GetFilePath(jValue, .sDefault="*unknown file*"), sFormattedMessage);
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
stock JSON:Json_GetRefValue(
    const JSON:jValue,
    sOut[] = NULL_STRING,
    const iOutLen = 0,
    &bool:bIsRef = false
) {
    bIsRef = false;

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
    
    Json__RightTrim(sWorkDir, '/');
    formatex(sFileName, charsmax(sFileName), "%s/%s.json", sWorkDir, sRefFilePath);

    if (iOutLen) {
        copy(sOut, iOutLen, sFileName);
    }

    bIsRef = true;
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

stock bool:Json_Object_GetBool(const JSON:jObj, const sKey[], const bool:bDefault = false, const bool:bDotNot = false) {
    if (!json_object_has_value(jObj, sKey, JSONBoolean, bDotNot)) {
        return bDefault;
    }
    
    return json_object_get_bool(jObj, sKey, bDotNot);
}

stock Json_Object_GetInt(const JSON:jObj, const sKey[], const iDefault = 0, const bool:bDotNot = false) {
    if (!json_object_has_value(jObj, sKey, JSONNumber, bDotNot)) {
        return iDefault;
    }
    
    return json_object_get_number(jObj, sKey, bDotNot);
}

stock Float:Json_Object_GetFloat(const JSON:jObj, const sKey[], const Float:fDefault = 0.0, const bool:bDotNot = false) {
    if (!json_object_has_value(jObj, sKey, JSONNumber, bDotNot)) {
        return fDefault;
    }
    
    return json_object_get_real(jObj, sKey, bDotNot);
}

stock Json_Object_GetString(
    const JSON:jObj,
    const sKey[],
    sOut[],
    const iOutLen,
    const sDefault[] = NULL_STRING,
    const bool:bDotNot = false
) {
    if (!json_object_has_value(jObj, sKey, JSONString, bDotNot)) {
        return copy(sOut, iOutLen, sDefault);
    } else {
        return json_object_get_string(jObj, sKey, sOut, iOutLen, bDotNot);
    }
}

stock Json_Object_GetStr(const JSON:jObj, const sKey[], const sDefault[] = NULL_STRING, const bool:bDotNot = false) {
    new sRes[MAX_FMT_LENGTH];
    Json_Object_GetString(jObj, sKey, sRes, charsmax(sRes), sDefault, bDotNot);
    
    return sRes;
}

stock Json_Object_GetColoredChatMessage(
    const JSON:jObj,
    const sKey[],
    sOut[],
    const iOutLen,
    const sDefault[] = NULL_STRING,
    const bool:bDotNot = false
) {
    new iRes = Json_Object_GetString(jObj, sKey, sOut, iOutLen, sDefault, bDotNot);
    
    iRes -= replace_all(sOut, iOutLen, "^^1", "^1");
    iRes -= replace_all(sOut, iOutLen, "^^3", "^3");
    iRes -= replace_all(sOut, iOutLen, "^^4", "^4");

    return iRes;
}

/*
Обёртка для кеширования ссылок.
! Очень извращённая штука, делал чисто под конкретные задачи !
! Но в целом, наверное, можно ещё где-то использовать !

Пример использования:

* new JSON:jRefed, T_ModuleUnit:iUnit = Invalid_ModuleUnit;
* Json_GetCachedRefValue_Begin(jModuleUnit, __cache_ModuleUnits, iUnit, jRefed)
* {
*     new sModuleUnit[S_ModuleUnit];
*     if (ModuleUnits_ReadFromJson(jRefed, sModuleUnit)) {
*         iUnit = ModuleUnits_Push(sModuleUnit);
*     }
* }
* Json_GetCachedRefValue_End(jModuleUnit, __cache_ModuleUnits, iUnit, jRefed)

Между *_Begin и *_End должен быть расположен код разбора
содержимого файла с последующей записью кешируемого значения.

Обе части в ходе выполнения могут прервать выполнение функции,
при чём вторая часть делает это в любом случае.
Также, это значит что код рабора файла может быть не вызван вовсе.

На строках обёртки ставить ; не надо.
*/

// (const JSON:jValue, const Trie:tCache, &any:iCachedValue, &JSON:jRefed)
#define Json_GetCachedRefValue_Begin(%1,%2,%3,%4) \
    new __Json_GetCachedRefValue_sJsonRef[PLATFORM_MAX_PATH]; \
    if ( \
        Json_IsRef(%1, __Json_GetCachedRefValue_sJsonRef, charsmax(__Json_GetCachedRefValue_sJsonRef)) \
        && TrieGetCell(%2, __Json_GetCachedRefValue_sJsonRef, %3) \
        || ((%4 = Json_GetRefValue(%1)) ? false : false) \
    ) { \
        return %3; \
    }

// (const JSON:jValue, const Trie:tCache, &any:iCachedValue, &JSON:jRefed)
#define Json_GetCachedRefValue_End(%1,%2,%3,%4) \
    (__Json_GetCachedRefValue_sJsonRef[0] \
        && TrieSetCell(%2, __Json_GetCachedRefValue_sJsonRef, %3)); \
    Json_FreeExIf(%4, bool:__Json_GetCachedRefValue_sJsonRef[0]); \
    return %3;
