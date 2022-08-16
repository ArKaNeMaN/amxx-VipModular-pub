#include <amxmodx>
#include <json>
#include "VipM/Utils"
#include "VipM/DebugMode"

public stock const PluginName[] = "Vip Modular";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "t.me/arkaneman";
public stock const PluginDescription[] = "Modular vip system";

#include "VipM/ModulesLimiter/Configs"

new const MODULES_CONFIG_FILE[] = "Modules";

new Trie:g_tModulesLimits = Invalid_Trie;

public VipM_OnInitModules() {
    RegisterPluginByVars();
    
    g_tModulesLimits = Configs_LoadModulesLimitsFromFile(MODULES_CONFIG_FILE, g_tModulesLimits);
}

public VipM_OnActivateModule(const sModuleName[]) {
    if (!TrieKeyExists(g_tModulesLimits, sModuleName)) {
        return VIPM_CONTINUE;
    }

    new Array:aLimits;
    TrieGetCell(g_tModulesLimits, sModuleName, aLimits);
    if (!VipM_Limits_ExecuteList(aLimits)) {
        return VIPM_STOP;
    }
    
    return VIPM_CONTINUE;
}
