#if defined _vipmodular_src_Vips_included
    #endinput
#endif
#define _vipmodular_src_Vips_included

/**
 * Vip Modular: Vips
 */

#include <amxmodx>

#define Vips_CheckAndAdd(%1) \
    CfgUnits_CheckAndAdd(%1, Vips)

#define Vips_Reset(%1) \
    ModuleUnits_ResetUser(%1)

Vips_UserUpdate(const UserId){
    Vips_Reset(UserId);
    Vips_CheckAndAdd(UserId);

    FwdExecP(UserUpdated, [UserId]);
}

#define CfgUnit_CheckAccess(%1,%2) \
    Limits_ExecuteUnitList(%2[CfgUnit_LimitUnits], %1, Limit_Exec_OR)

#define CfgUnit_AddToUser(%1,%2) \
    ModuleUnits_AddListToUser(%1, %2[CfgUnit_ModuleUnits])

CfgUnits_CheckAndAdd(const UserId, const Array:aUnits){
    if (aUnits == Invalid_Array) {
        return;
    }
    
    new Unit[S_CfgUnit];
    for (new i = 0; i < ArraySize(aUnits); i++) {
        ArrayGetArray(aUnits, i, Unit);

        CfgUnit_CheckAndAdd(UserId, Unit);
    }
}

bool:CfgUnit_CheckAndAdd(const UserId, const Unit[S_CfgUnit]){
    if (!CfgUnit_CheckAccess(UserId, Unit)) {
        return false;
    }
    
    CfgUnit_AddToUser(UserId, Unit);
    return true;
}
