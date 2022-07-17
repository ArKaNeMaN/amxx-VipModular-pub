#if defined _vipmodular_src_locUtils_included
    #endinput
#endif
#define _vipmodular_src_locUtils_included

/**
 * Vip Modular: Utils
 */

#include amxmodx
#include "VipM/ArrayTrieUtils"
#include "VipM/ArrayMap"

#include "VipM/Core/Modules/Main"
#include "VipM/Core/Limits/Main"
#include "VipM/Core/Structs"

PrintModulesData(const ArrayMap(amModules)) {
    server_print("╔═════╤══════════════════════════════════╤════════╗");
    server_print("║  #  │ Module name                      │ Status ║");
    server_print("╟─────┼──────────────────────────────────┼────────╢");
    ArrayMapForeachArray2 (amModules: i => Module[S_Module]) {
        server_print("║ %03d │ %-32s │ %-6s ║", i, Module[Module_Name], Module[Module_Enabled] ? "On" : "Off");
    }
    server_print("╟─────┴──────────────────────────────────┴────────╢");
    server_print("║ Total: %-5d                                    ║", ArrayMapSize(amModules));
    server_print("╚═════════════════════════════════════════════════╝");
}

PrintLimitsData(const ArrayMap(amLimits)) {
    server_print("╔═════╤══════════════════════════════════╤════════╤════════╤════════════╗");
    server_print("║  #  │ Limit name                       │ Params │ Static │ For player ║");
    server_print("╟─────┼──────────────────────────────────┼────────┼────────┼────────────╢");
    ArrayMapForeachArray2 (amLimits: i => Limit[S_Limit]) {
        server_print("║ %03d │ %-32s │ %-6d │ %-6s │ %-10s ║",
            i, Limit[Limit_Name],
            ArraySizeSafe(Limit[Limit_Params]),
            Limit[Limit_Static] ? "Yes" : "No",
            Limit[Limit_ForPlayer] ? "Yes" : "No"
        );
    }
    server_print("╟─────┴──────────────────────────────────┴────────┴────────┴────────────╢");
    server_print("║ Total: %-5d                                                          ║", ArrayMapSize(amLimits));
    server_print("╚═══════════════════════════════════════════════════════════════════════╝");
}

PrintConfigParams(const Array:Params) {
    server_print("╔═════╤══════════════════════════════════╤══════════╤══════════╗");
    server_print("║  #  │ Param name                       │ Type     │ Required ║");
    server_print("╟─────┼──────────────────────────────────┼──────────┼──────────╢");
    ArrayForeachArray2 (Params: i => Param[S_CfgParam]) {
        server_print("║ %3d │ %-32s │ %-8s │ %-8s ║",
            i, Param[CfgParam_Name],
            VIPM_PARAM_TYPE_NAMES[Param[CfgParam_Type]],
            Param[CfgParam_Required] ? "Yes" : "No"
        );
    }
    server_print("╟─────┴──────────────────────────────────┴──────────┴──────────╢");
    server_print("║ Total: %-5d                                                 ║", ArraySizeSafe(Params));
    server_print("╚══════════════════════════════════════════════════════════════╝");
}