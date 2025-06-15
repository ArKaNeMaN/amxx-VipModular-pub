#if defined __UTILS_JsonUtils_included
    #endinput
#endif
#define __UTILS_JsonUtils_included

#include <amxmodx>
#include <json>
#include <ParamsController>

#define JSON_UTILS__MAX_KEY_LENGTH PARAM_KEY_MAX_LEN
#define JSON_UTILS__MAX_STRING_LENGTH PARAM_VALUE_MAX_LEN

// deprecated
// Все параметры надо указывать явно
stock Trie:Json_ReadObjectToTrie(const JSON:jObject, &Trie:tTrie = Invalid_Trie) {
    if (tTrie == Invalid_Trie) {
        tTrie = TrieCreate();
    }
    
    if (!json_is_object(jObject)) {
        // А надо ли ошибку бросать?
        PCJson_ErrorForFile(jObject, "Json_ReadObjectToTrie: JSON-value must be an object.");
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

// TODO: Перенести в контроллер параметров
stock Json_Object_GetColoredChatMessage(
    const JSON:jObj,
    const sKey[],
    sOut[],
    const iOutLen,
    const sDefault[] = "",
    const bool:bDotNot = false
) {
    new iRes = PCSingle_ObjString(jObj, sKey, sOut, iOutLen, sDefault, bDotNot);
    
    iRes -= replace_all(sOut, iOutLen, "^^1", "^1");
    iRes -= replace_all(sOut, iOutLen, "^^3", "^3");
    iRes -= replace_all(sOut, iOutLen, "^^4", "^4");

    return iRes;
}
