#include <amxmodx>
#include <VipModular>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

enum E_ModuleParams {
    Param_Enabled = 0,
    Param_Override,
}

public stock const PluginName[] = "[VipM-M] Vip in TAB";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Vip modular`s module - VipInTab";

new const MODULE_NAME[] = "VipInTab";

new bool:gHasTag[MAX_PLAYERS + 1][E_ModuleParams];

public VipM_OnInitModules() {
    RegisterPluginByVars();

    VipM_Modules_Register(MODULE_NAME, true);
    VipM_SetModuleParams(MODULE_NAME,
        "Enabled", ptBoolean, true,
        "Override", ptBoolean, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
}

public VipM_OnUserUpdated(const UserId) {
    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);

    gHasTag[UserId][Param_Enabled] = VipM_Params_GetBool(Params, "Enabled", false);
    gHasTag[UserId][Param_Override] = VipM_Params_GetBool(Params, "Override", false);
}

@OnModuleActivate() {
    register_message(get_user_msgid("ScoreAttrib"), "@OnMsgScoreAttrib");
}

@OnMsgScoreAttrib(const MsgId, const MsgType, const MsgDest) {
    new UserId = get_msg_arg_int(1);
    if (!gHasTag[UserId][Param_Enabled]) {
        return;
    }

    if (
        !gHasTag[UserId][Param_Override]
        && get_msg_arg_int(2) != 0
    ) {
        return;
    }

    set_msg_arg_int(2, ARG_BYTE, (1<<2));
}
