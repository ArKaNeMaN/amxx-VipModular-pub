#include <amxmodx>
#include <json>
#include <VipModular>
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

Trie:Configs_LoadModulesLimitsFromFile(const sFileName[], Trie:tModules = Invalid_Trie) {
    TrieCreateIfNotCreated(tModules);

    new JSON:jFile = GET_FILE_JSON(sFileName);
    if (jFile == Invalid_JSON) {
        log_error(0, "Invalid JSON syntax. File `%s`.", GET_FILE_JSON_PATH(sFileName));
        return tModules;
    }

    if (!json_is_array(jFile)) {
        Json_LogForFile(jFile, "WARNING", "Root value must be an array.");
        Json_DeepFree(jFile);
        return tModules;
    }

    json_array_foreach_value (jFile: i => jItem) {
        if (!json_is_object(jItem)) {
            Json_LogForFile(jItem, "WARNING", "Array item #%d isn`t object.", i);
            json_free(jItem);
            continue;
        }

        new JSON:jLimits = json_object_get_value(jItem, "Limits");
        new Array:aLimits = VipM_Limits_ReadListFromJson(jLimits);
        if (!ArraySizeSafe(aLimits)) {
            Json_LogForFile(jItem, "WARNING", "Field `Limits` must have 1 or more items.");
            json_free(jItem);
        }
        json_free(jLimits);


        new Array:aModuleNames = json_object_get_strings_list(jItem, "Modules", VIPM_MODULE_MAX_NAME);
        if (!ArraySizeSafe(aModuleNames)) {
            Json_LogForFile(jItem, "WARNING", "Field `Modules` must have 1 or more items.");
            continue;
        }

        ArrayForeachString (aModuleNames: j => sModuleName[VIPM_MODULE_MAX_NAME]) {
            if (TrieKeyExists(tModules, sModuleName)) {
                Json_LogForFile(jItem, "WARNING", "Duplicate limits for module `%s`.", sModuleName);
                continue;
            }

            TrieSetCell(tModules, sModuleName, aLimits);
        }
        
        json_free(jItem);
        ArrayDestroy(aModuleNames);
    }

    Json_DeepFree(jFile);
    return tModules;
}
