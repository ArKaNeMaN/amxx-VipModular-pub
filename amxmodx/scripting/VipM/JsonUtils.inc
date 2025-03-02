#if defined __UTILS_JsonUtils_included
    #endinput
#endif
#define __UTILS_JsonUtils_included

#include <amxmodx>
#include <json>
#include <ParamsController>

#define JSON_UTILS__MAX_KEY_LENGTH 128
#define JSON_UTILS__MAX_STRING_LENGTH 256
#define JSON_UTILS__MAX_REF_LENGTH 128

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
    const sDefault[] = "",
    const bool:bDotNot = false
) {
    if (!json_object_has_value(jObj, sKey, JSONString, bDotNot)) {
        return copy(sOut, iOutLen, sDefault);
    } else {
        return json_object_get_string(jObj, sKey, sOut, iOutLen, bDotNot);
    }
}

stock Json_Object_GetStr(const JSON:jObj, const sKey[], const sDefault[] = "", const bool:bDotNot = false) {
    new sRes[MAX_FMT_LENGTH];
    Json_Object_GetString(jObj, sKey, sRes, charsmax(sRes), sDefault, bDotNot);
    
    return sRes;
}

stock Json_Object_GetColoredChatMessage(
    const JSON:jObj,
    const sKey[],
    sOut[],
    const iOutLen,
    const sDefault[] = "",
    const bool:bDotNot = false
) {
    new iRes = Json_Object_GetString(jObj, sKey, sOut, iOutLen, sDefault, bDotNot);
    
    iRes -= replace_all(sOut, iOutLen, "^^1", "^1");
    iRes -= replace_all(sOut, iOutLen, "^^3", "^3");
    iRes -= replace_all(sOut, iOutLen, "^^4", "^4");

    return iRes;
}
