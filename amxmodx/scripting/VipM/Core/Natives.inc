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

#include "VipM/Core/API/Limits"
#include "VipM/Core/API/Modules"

public plugin_natives() {
    Natives_Init("VipM");

    Natives_Reg("UserUpdate");
    Natives_Reg("Json_LogForFile");

    Natives_Modules_Init();
    API_Limits_Init();
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
