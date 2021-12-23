#if defined _vipmodular_src_locUtils_included
    #endinput
#endif
#define _vipmodular_src_locUtils_included

#include "VipM/Core/Modules.inc"
#include "VipM/Core/Limits.inc"

/**
 * Vip Modular: Utils
 */

#include amxmodx

PrintModulesData(const ArrayMap(amModules)){
    server_print("╔═════╤══════════════════════════════════╤════════╗");
    server_print("║  #  │ Module name                      │ Status ║");
    server_print("╟─────┼──────────────────────────────────┼────────╢");
    ArrayMapForeachArray(amModules => Module[S_Module])
        server_print("║ %03d │ %-32s │ %-6s ║", __i__, Module[Module_Name], Module[Module_Enabled] ? "On" : "Off");
    server_print("╟─────┴──────────────────────────────────┴────────╢");
    server_print("║ Total: %-5d                                    ║", ArrayMapSize(amModules));
    server_print("╚═════════════════════════════════════════════════╝");
}

PrintLimitsData(const ArrayMap(amLimits)){
    server_print("╔═════╤══════════════════════════════════╤════════╤════════╤════════════╗");
    server_print("║  #  │ Limit name                       │ Params │ Static │ For player ║");
    server_print("╟─────┼──────────────────────────────────┼────────┼────────┼────────────╢");
    ArrayMapForeachArray(amLimits => Limit[S_Limit])
        server_print("║ %03d │ %-32s │ %-6d │ %-6s │ %-10s ║",
            __i__, Limit[Limit_Name],
            ArraySizeSafe(Limit[Limit_Params]),
            Limit[Limit_Static] ? "Yes" : "No",
            Limit[Limit_ForPlayer] ? "Yes" : "No"
        );
    server_print("╟─────┴──────────────────────────────────┴────────┴────────┴────────────╢");
    server_print("║ Total: %-5d                                                          ║", ArrayMapSize(amLimits));
    server_print("╚═══════════════════════════════════════════════════════════════════════╝");
}

PrintConfigParams(const Array:Params){
    server_print("╔═════╤══════════════════════════════════╤══════════╤══════════╗");
    server_print("║  #  │ Param name                       │ Type     │ Required ║");
    server_print("╟─────┼──────────────────────────────────┼──────────┼──────────╢");
    ArrayForeachArray(Params => Param[S_CfgParam])
        server_print("║ %3d │ %-32s │ %-8s │ %-8s ║",
            __i__, Param[CfgParam_Name],
            VIPM_PARAM_TYPE_NAMES[Param[CfgParam_Type]],
            Param[CfgParam_Required] ? "Yes" : "No"
        );
    server_print("╟─────┴──────────────────────────────────┴──────────┴──────────╢");
    server_print("║ Total: %-5d                                                 ║", ArraySizeSafe(Params));
    server_print("╚══════════════════════════════════════════════════════════════╝");
}