#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

#define RELOAD_ON_PLAYER_SPAWN 1
#define RELOAD_ON_ROUND_START 1
#define RELOAD_ON_ROUND_END 1

public stock const PluginName[] = "[VipM] Misc";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Auto reload player's privilegies.";

public VipM_OnLoaded() {
    RegisterPluginByVars();
    
    #if RELOAD_ON_PLAYER_SPAWN
        RegisterHookChain(RG_CBasePlayer_Spawn, "@ReloadPlayer", false);
    #endif
    
    #if RELOAD_ON_ROUND_START
        RegisterHookChain(RG_CSGameRules_RestartRound, "@OnPlayerSpawned", false);
    #endif
    
    #if RELOAD_ON_ROUND_END
        RegisterHookChain(RG_RoundEnd, "@OnPlayerSpawned", false);
    #endif

    register_srvcmd("vipm_update_users", "@SrvCmd_ReloadPlayers");
    register_srvcmd("vipm_reload_players", "@SrvCmd_ReloadPlayers");
}

@ReloadPlayer(const UserId) {
    VipM_UserUpdate(UserId);
}

@ReloadAllPlayers() {
    for (new UserId = 1; UserId <= MAX_PLAYERS; UserId++) {
        if (is_user_connected(UserId) && !is_user_bot(UserId) && !is_user_hltv(UserId)) {
            @ReloadPlayer(UserId);
        }
    }
}

@SrvCmd_ReloadPlayers() {
    @ReloadAllPlayers();
}

@SrvCmd_ReloadPlayer() {
    enum {Arg_UserId = 1}

    if (read_argc() < Arg_UserId) {
        server_print("Invalid command params.");
        server_print("Usage: vipm_reload_players <UserId>");
        return;
    }

    new UserId = read_argv_int(Arg_UserId);

    @ReloadPlayer(UserId);
}
