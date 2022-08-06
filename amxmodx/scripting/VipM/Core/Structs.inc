#if defined _vipmodular_src_Structs_included
    #endinput
#endif
#define _vipmodular_src_Structs_included

/**
 * Vip Modular: Structs
 */

enum _:S_CfgParam{
    CfgParam_Name[32],
    E_ParamType:CfgParam_Type,
    bool:CfgParam_Required,
}
