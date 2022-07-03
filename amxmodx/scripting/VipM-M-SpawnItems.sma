#include <amxmodx>
#include <json>
#include <reapi>
#include <VipModular>

#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][M] Spawn Items";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Vip modular`s module - Spawn Items";

new const MODULE_NAME[] = "SpawnItems";

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);
    
    VipM_Modules_Register(MODULE_NAME, true);
    VipM_Modules_AddParams(MODULE_NAME,
        "Items", ptCustom, false,
        "Limits", ptLimits, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnRead, "@OnReadConfig");
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
}

@OnReadConfig(const JSON:jCfg, Trie:Params){
    if(!json_object_has_value(jCfg, "Items")){
        log_amx("[WARNING] Field `Items` required.");
        return VIPM_STOP;
    }
    
    new JSON:jItems = json_object_get_value(jCfg, "Items");
    new Array:aItems = VipM_IC_JsonGetItems(jItems);
    json_free(jItems);

    if(ArraySizeSafe(aItems) < 1){
        ArrayDestroySafe(aItems);
        log_amx("[WARNING] Field `Items` is empty.");
        return VIPM_STOP;
    }
    TrieSetCell(Params, "Items", aItems);

    return VIPM_CONTINUE;
}

@OnModuleActivate(){
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawned", true);
}

@OnPlayerSpawned(const UserId){
    if (!is_user_alive(UserId)) {
        return;
    }

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    if (Params == Invalid_Trie) {
        return;
    }

    if (!VipM_Params_ExecuteLimitsList(Params, "Limits", UserId, Limit_Exec_AND)) {
        return;
    }

    new Array:aItems = Array:VipM_Params_GetInt(Params, "Items", _:Invalid_Array);
    if (aItems == Invalid_Array) {
        return;
    }
    
    VipM_IC_GiveItems(UserId, aItems);
}
