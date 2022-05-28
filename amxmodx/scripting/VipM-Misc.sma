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
