#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM] Misc";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Some extentions for Vip Modular.";

public VipM_OnLoaded(){
    RegisterPluginByVars();
    
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawned", false);

    register_srvcmd("vipm_update_users", "@SrvCmd_UpdateUsers");
}

@OnPlayerSpawned(const UserId){
    VipM_UserUpdate(UserId);
}

@SrvCmd_UpdateUsers() {
    for (new UserId = 1; UserId <= MAX_PLAYERS; UserId++) {
        if (is_user_connected(UserId) && !is_user_bot(UserId)) {
            VipM_UserUpdate(UserId);
        }
    }
}
