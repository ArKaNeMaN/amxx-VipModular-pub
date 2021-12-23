#include <amxmodx>
#include <VipModular>
#include "VipM/DebugMode"
#include "VipM/ArrayTrieUtils"
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "Vip Modular";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "t.me/arkaneman";
public stock const PluginDescription[] = "Modular vip system";

new Array:Vips; // S_CfgUnit
new Trie:gUserVip[MAX_PLAYERS + 1] = {Invalid_Trie, ...}; // ModuleName => Trie:Params

#include "VipM/Core/Structs"
#include "VipM/Core/Forwards"
#include "VipM/Core/Utils"
#include "VipM/Core/Limits"
#include "VipM/Core/Vips"

#include "VipM/Core/SrvCmds"
#include "VipM/Core/Configs"
#include "VipM/Core/Natives"

public plugin_precache(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);
    register_library(VIPM_LIBRARY);
    
    Fwds_Init();
    SrvCmds_Init();
    Limits_Init();
    Modules_Init();
    FwdExec(InitModules);

    Cfg_LoadModulesConfig();
    Vips = Cfg_ReadVipConfigs();

    server_print("[%s v%s] Loaded %d config units.", PluginName, VIPM_VERSION, ArraySizeSafe(Vips));
    FwdExec(Loaded);

    Dbg_PrintServer("Vip Modular run in debug mode!");
}

public client_disconnected(UserId){
    Vips_Reset(UserId);
}

public client_putinserver(UserId){
    if(is_user_bot(UserId))
        return;

    Vips_UserUpdate(UserId);
}
