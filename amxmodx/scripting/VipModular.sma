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
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Modular vip system";

#include "VipM/Core/Objects/Modules/Type"
#include "VipM/Core/VipsManager"
#include "VipM/Core/SrvCmds"

public plugin_precache() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_library(VIPM_LIBRARY);
    CreateConstCvar("vipm_version", PluginVersion);

    Forwards_Init("VipM");
    Forwards_Reg("ReadUnit", ET_IGNORE, FP_CELL, FP_CELL);

    VipsManager_Init();
    SrvCmds_Init();
    
    VipsManager_SetRootDir(VipM_iGetCfgPath(""));
    VipsManager_LoadFromFile(VipM_iGetCfgPath("Vips.json"));
    VipsManager_LoadFromFolder(VipM_iGetCfgPath("Vips"));

    ModuleType_ActivateUsed();

    Forwards_RegAndCall("Loaded", ET_IGNORE);
    server_print("[%s v%s] Loaded %d config units.", PluginName, PluginVersion, VipsManager_VipsCount());
    Dbg_PrintServer("Vip Modular run in debug mode!");
}

public client_disconnected(UserId) {
    VipsManager_UserReset(UserId);
}

public client_putinserver(UserId) {
    if (is_user_bot(UserId)) {
        return;
    }

    RequestFrame("@CallUserUpdate", UserId);
}

@CallUserUpdate(UserId) {
    if (is_user_connected(UserId)) {
        VipsManager_UserReload(UserId);
    }
}

#include "VipM/Core/API/Main"
#include "VipM/Core/API/Limits"
#include "VipM/Core/API/Modules"
public plugin_natives() {
    API_Main_Init();
    API_Limits_Init();
    API_Modules_Init();
}
