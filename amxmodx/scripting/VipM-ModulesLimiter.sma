#include <amxmodx>
#include <json>
#include "VipM/Utils"
#include "VipM/DebugMode"

public stock const PluginName[] = "[VipM] Modules Limiter";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Modules activation controller";

#include "VipM/ModulesLimiter/Configs"

new const MODULES_CONFIG_FILE[] = "Modules";

new Trie:g_tModulesLimits = Invalid_Trie;

public VipM_OnInitModules() {
    RegisterPluginByVars();
    
    g_tModulesLimits = Configs_LoadModulesLimitsFromFile(MODULES_CONFIG_FILE, g_tModulesLimits);
}

public VipM_OnActivateModule(const sModuleName[]) {
    if (!TrieKeyExists(g_tModulesLimits, sModuleName)) {
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
