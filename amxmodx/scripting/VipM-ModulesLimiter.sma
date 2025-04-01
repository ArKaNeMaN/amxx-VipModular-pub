#include <amxmodx>
#include <json>
#include <VipModular>
#include "VipM/Utils"
#include "VipM/DebugMode"
#include "VipM/ArrayTrieUtils"

public stock const PluginName[] = "[VipM] Modules Limiter";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Modules activation controller";

new const MODULES_CONFIG_FILE[] = "Modules";

new Trie:g_tModulesLimits = Invalid_Trie;

public VipM_OnLoaded() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    
    g_tModulesLimits = LoadModulesLimitsFromFile(MODULES_CONFIG_FILE, g_tModulesLimits);
}

public VipM_Modules_OnActivate(const sModuleName[]) {
    if (
        g_tModulesLimits == Invalid_Trie
        || !TrieKeyExists(g_tModulesLimits, sModuleName)
    ) {
        Dbg_Log("Module `%s` activated. (!TrieKeyExists)", sModuleName);
        return VIPM_CONTINUE;
    }

    new Array:aLimits;
    TrieGetCell(g_tModulesLimits, sModuleName, aLimits);
    if (!VipM_Limits_ExecuteList(aLimits)) {
        Dbg_Log("Module `%s` not ativated.", sModuleName);
        return VIPM_STOP;
    }
    
    Dbg_Log("Module `%s` activated. (Limits passed)", sModuleName);
    
    return VIPM_CONTINUE;
}

Trie:LoadModulesLimitsFromFile(const sFileName[], Trie:tModules = Invalid_Trie) {
    if (tModules == Invalid_Trie) {
        tModules = TrieCreate();
    }

    new JSON:jFile = PCJson_ParseFile(GET_FILE_JSON_PATH(sFileName));
    if (jFile == Invalid_JSON) {
        log_error(0, "Invalid JSON syntax. File `%s`.", GET_FILE_JSON_PATH(sFileName));
        return tModules;
    }

    if (!json_is_array(jFile)) {
        PCJson_LogForFile(jFile, "WARNING", "Root value must be an array.");
        PCJson_Free(jFile);
        return tModules;
    }

    json_array_foreach_value (jFile: i => jItem) {
        if (!json_is_object(jItem)) {
            PCJson_LogForFile(jItem, "WARNING", "Array item #%d isn`t object.", i);
            json_free(jItem);
            continue;
        }

        new JSON:jLimits = json_object_get_value(jItem, "Limits");
        new Array:aLimits = VipM_Limits_ReadListFromJson(jLimits);
        json_free(jLimits);
        if (!ArraySizeSafe(aLimits)) {
            PCJson_LogForFile(jItem, "WARNING", "Field `Limits` must have 1 or more items.");
            json_free(jItem);
            continue;
        }

        new Array:aModuleNames = json_object_get_strings_list(jItem, "Modules", VIPM_MODULES_TYPE_NAME_MAX_LEN);
        if (!ArraySizeSafe(aModuleNames)) {
            PCJson_LogForFile(jItem, "WARNING", "Field `Modules` must have 1 or more items.");
            continue;
        }

        ArrayForeachString (aModuleNames: j => sModuleName[VIPM_MODULES_TYPE_NAME_MAX_LEN]) {
            if (TrieKeyExists(tModules, sModuleName)) {
                PCJson_LogForFile(jItem, "WARNING", "Duplicate limits for module `%s`.", sModuleName);
                continue;
            }

            TrieSetCell(tModules, sModuleName, aLimits);
        }
        
        json_free(jItem);
        ArrayDestroy(aModuleNames);
    }

    PCJson_Free(jFile);
    return tModules;
}
