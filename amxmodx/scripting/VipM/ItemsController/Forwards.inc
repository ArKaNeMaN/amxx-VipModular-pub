/**
 * Vip Modular: Forwards
 */

#include amxmodx

new const __FWD_NAME_TPL[] = "VipM_IC_On%s";

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

#define FwdRetInit(%1) \
    __FWD_TEMP_INT = %1

enum E_Fwds {
    Fwd_InitTypes,
    Fwd_Loaded,
    Fwd_GiveItem,
    Fwd_ReadItem,
}
new Fwds[E_Fwds];

Fwds_Init(){
    FwdReg(InitTypes, "InitTypes", ET_IGNORE);
    FwdReg(Loaded, "Loaded", ET_IGNORE);
    FwdRegP(GiveItem, "GiveItem", ET_STOP, [FP_CELL, FP_CELL]);
    FwdRegP(ReadItem, "ReadItem", ET_STOP, [FP_CELL, FP_CELL]);
}
