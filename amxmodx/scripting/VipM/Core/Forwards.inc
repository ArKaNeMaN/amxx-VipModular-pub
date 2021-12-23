#if defined _vipmodular_src_Forwards_included
    #endinput
#endif
#define _vipmodular_src_Forwards_included

/**
 * Vip Modular: Forwards
 */

#include amxmodx

new const __FWD_NAME_TPL[] = "VipM_On%s";

new __FWD_TEMP_INT;
#define FwdExecP(%1,[%2]) \
    ExecuteForward(Fwds[Fwd_%1],__FWD_TEMP_INT,%2)
#define FwdRegP(%1,%2,%3,[%4]) \
    Fwds[Fwd_%1] = CreateMultiForward(fmt(__FWD_NAME_TPL, %2), %3, %4)

#define FwdExec(%1) \
    ExecuteForward(Fwds[Fwd_%1],__FWD_TEMP_INT)
#define FwdReg(%1,%2,%3) \
    Fwds[Fwd_%1] = CreateMultiForward(fmt(__FWD_NAME_TPL, %2), %3)

#define FwdRet() \
    __FWD_TEMP_INT

enum E_Fwds {
    Fwd_InitModules,
    Fwd_Loaded,
    Fwd_UserUpdated,

    Fwd_ReadUnit,
    Fwd_ReadModuleUnit,
    Fwd_ReadLimitUnit,
}
new Fwds[E_Fwds];

Fwds_Init(){
    FwdReg(InitModules, "InitModules", ET_IGNORE);
    FwdReg(Loaded, "Loaded", ET_IGNORE);
    FwdRegP(UserUpdated, "UserUpdated", ET_IGNORE, [FP_CELL]);
    
    FwdRegP(ReadUnit, "ReadUnit", ET_IGNORE, [FP_CELL, FP_CELL]);
    FwdRegP(ReadModuleUnit, "ReadModuleUnit", ET_IGNORE, [FP_CELL, FP_CELL]);
    FwdRegP(ReadLimitUnit, "ReadLimitUnit", ET_IGNORE, [FP_CELL, FP_CELL]);
}
