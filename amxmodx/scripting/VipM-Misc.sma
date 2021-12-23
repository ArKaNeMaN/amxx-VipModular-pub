#include <amxmodx>
#include <reapi>
#include <VipModular>

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM] Misc";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Some extentions for Vip Modular.";

public VipM_OnLoaded(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);
    
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawned", false);
}

@OnPlayerSpawned(const UserId){
    VipM_UserUpdate(UserId);
}
