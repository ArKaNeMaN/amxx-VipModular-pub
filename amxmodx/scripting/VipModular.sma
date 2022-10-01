#include <amxmodx>
#include <VipModular>
#include "VipM/DebugMode"
#include "VipM/ArrayTrieUtils"
#include "VipM/Utils"
#include "VipM/Forwards"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "Vip Modular";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "t.me/arkaneman";
public stock const PluginDescription[] = "Modular vip system";

new Array:Vips; // S_CfgUnit
new Trie:gUserVip[MAX_PLAYERS + 1] = {Invalid_Trie, ...}; // ModuleName => Trie:Params

#include "VipM/Core/Utils"

#include "VipM/Core/Structs"
#include "VipM/Core/Modules/Main"
#include "VipM/Core/Limits/Main"
#include "VipM/Core/Configs/Main"
#include "VipM/Core/Vips"

#include "VipM/Core/SrvCmds"
#include "VipM/Core/Natives"

public plugin_precache() {
    RegisterPluginByVars();
    register_library(VIPM_LIBRARY);
    CreateConstCvar("vipm_version", VIPM_VERSION);

    RegisterForwards();
    SrvCmds_Init();
    Limits_Init();
    Modules_Init();
    Forwards_RegAndCall("InitModules", ET_IGNORE);

    Vips = Cfg_LoadVipsConfigs();

    if (!ArraySizeSafe(Vips)) {
        set_fail_state("Vips configs not found.");
    }

    server_print("[%s v%s] Loaded %d config units.", PluginName, VIPM_VERSION, ArraySizeSafe(Vips));
    Forwards_RegAndCall("Loaded", ET_IGNORE);

    Modules_EnableAllUsed();

    Dbg_PrintServer("Vip Modular run in debug mode!");
}

RegisterForwards() {
    Forwards_Init("VipM");
    Forwards_Reg("UserUpdated", ET_IGNORE, FP_CELL);
    Forwards_Reg("ReadUnit", ET_IGNORE, FP_CELL, FP_CELL);
    Forwards_Reg("ActivateModule", ET_STOP, FP_STRING);
    Forwards_Reg("ReadModuleUnit", ET_IGNORE, FP_CELL, FP_CELL);
    Forwards_Reg("ReadLimitUnit", ET_IGNORE, FP_CELL, FP_CELL);
}

public client_disconnected(UserId) {
    Vips_Reset(UserId);
}

public client_putinserver(UserId) {
    if (is_user_bot(UserId)) {
        return;
    }

    Vips_UserUpdate(UserId);
}
