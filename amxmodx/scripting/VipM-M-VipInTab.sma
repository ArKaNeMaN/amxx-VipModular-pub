#include <amxmodx>
#include <VipModular>

#pragma semicolon 1
#pragma compress 1

enum _:E_PARAMS{
    P_ENABLED = 0,
    P_OVERRIDE,
}

public stock const PluginName[] = "[VipM][M] Vip in TAB";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Vip modular`s module - VipInTab";

new const MODULE_NAME[] = "VipInTab";

new bool:gHasTag[MAX_PLAYERS + 1][E_PARAMS];

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);

    VipM_Modules_Register(MODULE_NAME, true);
    VipM_SetModuleParams(MODULE_NAME,
        "Enabled", ptBoolean, true,
        "Override", ptBoolean, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
}

public VipM_OnUserUpdated(const UserId){
    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);

    gHasTag[UserId][P_ENABLED] = VipM_Params_GetBool(Params, "Enabled", false);
    gHasTag[UserId][P_OVERRIDE] = VipM_Params_GetBool(Params, "Override", false);
}

@OnModuleActivate(){
    register_message(get_user_msgid("ScoreAttrib"), "@OnMsgScoreAttrib");
}

@OnMsgScoreAttrib(const MsgId, const MsgType, const MsgDest){
    new UserId = get_msg_arg_int(1);
    if(!gHasTag[UserId][P_ENABLED])
        return;

    if(
        !gHasTag[UserId][P_OVERRIDE]
        && get_msg_arg_int(2) != 0
    ) return;

    set_msg_arg_int(2, ARG_BYTE, (1<<2));
}
