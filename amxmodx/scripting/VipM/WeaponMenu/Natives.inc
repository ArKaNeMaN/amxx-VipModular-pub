#include <amxmodx>
#include "VipM/Natives"

public plugin_natives() {
    Natives_Init("VipM_WeaponMenu");

    Natives_Reg("SetExpireStatus");
}

@_SetExpireStatus() {
    enum {Arg_UserId = 1, Arg_sStatus};

    get_string(Arg_sStatus, gUserExpireStatus[get_param(Arg_UserId)], charsmax(gUserExpireStatus[]));
}
