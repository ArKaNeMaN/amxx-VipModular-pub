#if defined _vipmodular_src_Natives_included
    #endinput
#endif
#define _vipmodular_src_Natives_included

/**
 * Vip Modular: Natives
 */

#include <amxmodx>
#include "VipM/Natives"
#include "VipM/Utils"

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
    Natives_Reg("Json_LogForFile");

    Natives_Modules_Init();
    Natives_Limits_Init();
}

// native VipM_Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...);
@_Json_LogForFile() {
    enum {Arg_jValue = 1, Arg_sPrefix, Arg_sMessage, Arg_fmtArgs}

    static sMessage[512], sPrefix[32];
    vdformat(sMessage, charsmax(sMessage), Arg_sMessage, Arg_fmtArgs);
    get_string(Arg_sPrefix, sPrefix, charsmax(sPrefix));

    Json_LogForFile(JSON:get_param(Arg_jValue), sPrefix, sMessage);
}

@_UserUpdate() {
    enum {Arg_UserId = 1}

    new UserId = get_param(Arg_UserId);
    if (!is_user_connected(UserId)) {
        return;
    }
    
    Vips_UserUpdate(UserId);
}
