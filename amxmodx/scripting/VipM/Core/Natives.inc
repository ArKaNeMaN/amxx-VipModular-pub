#if defined _vipmodular_src_Natives_included
    #endinput
#endif
#define _vipmodular_src_Natives_included

/**
 * Vip Modular: Natives
 */

#include <amxmodx>
#include "VipM/Natives"

Array:Native_GetParamsList(const NullArg, const ParamsCount, Array:aParams = Invalid_Array) {
    const ARGS_NUM_PER_PARAM = 3;

    if ((ParamsCount - NullArg) % ARGS_NUM_PER_PARAM != 0) {
        log_error(1, "[ERROR] Invalid parameters num.");
        return Invalid_Array;
    }

    if (aParams == Invalid_Array) {
        aParams = ArrayCreate(S_CfgParam, max(1, (ParamsCount - NullArg) / ARGS_NUM_PER_PARAM));
    }

    new Param[S_CfgParam];
    for (new i = NullArg+1; i < ParamsCount; i += ARGS_NUM_PER_PARAM) {
        get_string(i, Param[CfgParam_Name], charsmax(Param[CfgParam_Name]));
        Param[CfgParam_Type] = E_ParamType:get_param_byref(i+1);
        Param[CfgParam_Required] = bool:get_param_byref(i+2);
        
        // Dbg_PrintServer("^t- %s: %d, %s", Param[CfgParam_Name], _:Param[CfgParam_Type], Param[CfgParam_Required] ? "+" : "-");

        ArrayPushArray(aParams, Param);
    }
    return aParams;
}

#include "VipM/Core/Natives/Limits.inc"
#include "VipM/Core/Natives/Modules.inc"

public plugin_natives() {
    Natives_Init("VipM");

    Natives_Reg("UserUpdate");

    Natives_Modules_Init();
    Natives_Limits_Init();
}

@_UserUpdate(const PluginId) {
    enum {Arg_UserId = 1}

    new UserId = get_param(Arg_UserId);
    if (!is_user_connected(UserId)) {
        return;
    }
    
    Vips_UserUpdate(UserId);
}
