#include <amxmodx>
#include <json>
#include <VipModular>
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

Trie:Configs_LoadModulesLimitsFromFile(const sFileName[], Trie:tModules = Invalid_Trie) {
    new JSON:jFile = GET_FILE_JSON(sFileName);
    if (!json_is_array(jFile)) {
        Json_LogForFile(jFile, "WARNING", "Root value must be an array.");
        Json_DeepFree(jFile);
        return tModules;
    }

    TrieCreateIfNotCreated(tModules);

    json_array_foreach_value (jFile: i => jItem) {
        if (!json_is_object(jItem)) {
            Json_LogForFile(jItem, "WARNING", "Array item #%d isn`t object.", i);
            json_free(jItem);
            continue;
        }

        new JSON:jLimits = json_object_get_value(jItem, "Limits");
        new Array:aLimits = VipM_Limits_ReadListFromJson(jLimits);
        json_free(jLimits);

        if (!ArraySizeSafe(aLimits)) {
            Json_LogForFile(jItem, "WARNING", "Field `Limits` must have 1 or more items.");
            json_free(jItem);
        }

        new Array:aModuleNames = json_object_get_strings_list(jItem, "Modules", VIPM_MODULE_MAX_NAME);
        json_free(jItem);

        if (!ArraySizeSafe(aModuleNames)) {
            Json_LogForFile(jItem, "WARNING", "Field `Modules` must have 1 or more items.");
            continue;
        }

        ArrayForeachString (aModuleNames: j => sModuleName[VIPM_MODULE_MAX_NAME]) {
            TrieSetCell(tModules, sModuleName, aLimits);
        }
        ArrayDestroy(aModuleNames);
    }

    Json_DeepFree(jFile);
    return tModules;
}
