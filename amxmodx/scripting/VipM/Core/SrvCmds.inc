#if defined _vipmodular_src_SrvCmds_included
    #endinput
#endif
#define _vipmodular_src_SrvCmds_included

#include <amxmodx>
#include "VipM/ArrayTrieUtils"
#include "VipM/ArrayMap"

#include "VipM/Core/Objects/Modules/Type"
#include "VipM/Core/Objects/Modules/Unit"
#include "VipM/Core/Objects/Limits/Type"
#include "VipM/Core/Objects/Limits/Unit"
#include "VipM/Core/Objects/Param"

SrvCmds_Init() {
    register_srvcmd("vipm_info", "@SrvCmd_Info");
    
    register_srvcmd("vipm_modules", "@SrvCmd_Modules");
    register_srvcmd("vipm_module_params", "@SrvCmd_ModuleParams");

    register_srvcmd("vipm_limits", "@SrvCmd_Limits");
    register_srvcmd("vipm_limit_params", "@SrvCmd_LimitParams");
}

@SrvCmd_Info() {
    server_print("╓─────────");
    server_print("║ %s v%s by %s:", PluginName, VIPM_VERSION, PluginAuthor);
    server_print("║ │");
    server_print("║ ├─Contacts:");
    server_print("║ │ ├─GitHub: https://github.com/akraneman");
    server_print("║ │ ├─DevCS: https://dev-cs.ru/members/949/");
    server_print("║ │ ├─VK: https://vk.com/akraneman");
    server_print("║ │ └─TG: https://t.me/arkaneman");
    server_print("║ ├─Support:");
    server_print("║ │ ├─GitHub Issues: https://github.com/ArKaNeMaN/amxx-VipModular-pub/issues");
    server_print("║ │ ├─DevCS Resource: https://dev-cs.ru/resources/1466/");
    server_print("║ │ └─TG Forum: https://t.me/arkanaplugins/5");
    server_print("║ │");
    server_print("║ ├─Stats:");
    server_print("║ │ ├─Modules:");
    server_print("║ │ │ ├─Modules count: %d", ModuleType_GetCount());
    server_print("║ │ │ └─Module units count: %d", ModuleUnit_GetCount());
    server_print("║ │ ├─Limits:");
    server_print("║ │ │ ├─Types count: %d", LimitType_GetCount());
    server_print("║ │ │ └─Limit units count: %d", LimitUnit_GetCount());
    server_print("║ │ ├─Vips:");
    server_print("║ │ │ └─Vips count: %d", VipUnit_GetCount());
    server_print("║ │ └─Compiled by AmxModX v%s:", AMXX_VERSION_STR);
    server_print("║ │");
    server_print("║ └─GitHub repo: https://github.com/ArKaNeMaN/amxx-VipModular-pub");
    server_print("╙─────────");
}

@SrvCmd_Modules() {
    server_print("╔═════╤══════════════════════════════════╤════════╗");
    server_print("║  #  │ Module name                      │ Status ║");
    server_print("╟─────┼──────────────────────────────────┼────────╢");
    new Array:aModuleTypes = ModuleType_GetAll();
    ArrayForeachArray2 (aModuleTypes: i => ModuleType[S_ModuleType]) {
        server_print("║ %03d │ %-32s │ %-6s ║", i + 1, ModuleType[ModuleType_Name], ModuleType[ModuleType_Active] ? "On" : "Off");
    }
    server_print("╟─────┴──────────────────────────────────┴────────╢");
    server_print("║ Total: %-5d                                    ║", ModuleType_GetCount());
    server_print("╚═════════════════════════════════════════════════╝");

    return PLUGIN_HANDLED;
}

@SrvCmd_ModuleParams() {
    new sModuleTypeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    read_argv(1, sModuleTypeName, charsmax(sModuleTypeName));

    new T_ModuleType:iModuleType = ModuleType_Find(sModuleTypeName);
    if (iModuleType == Invalid_ModuleType) {
        server_print("Module type '%s' not found.", sModuleTypeName);
        return PLUGIN_HANDLED;
    }
    
    new Array:aParams = ModuleType_GetParams(iModuleType);
    PrintConfigParams(aParams);
    ArrayDestroy(aParams);

    return PLUGIN_HANDLED;
}

@SrvCmd_Limits() {
    server_print("╔═════╤══════════════════════════════════╤════════╤════════╤════════════╗");
    server_print("║  #  │ Limit name                       │ Params │ Static │ For player ║");
    server_print("╟─────┼──────────────────────────────────┼────────┼────────┼────────────╢");

    new Array:aLimitTypes = LimitType_GetAll();
    ArrayForeachArray2 (aLimitTypes: i => LimitType[S_LimitType]) {
        server_print("║ %03d │ %-32s │ %-6d │ %-6s │ %-10s ║",
            i + 1, LimitType[LimitType_Name],
            ArraySizeSafe(LimitType[LimitType_Params]),
            LimitType[LimitType_Static] ? "Yes" : "No",
            LimitType[LimitType_ForPlayer] ? "Yes" : "No"
        );
    }
    ArrayDestroy(aLimitTypes);

    server_print("╟─────┴──────────────────────────────────┴────────┴────────┴────────────╢");
    server_print("║ Total: %-5d                                                          ║", LimitType_GetCount());
    server_print("╚═══════════════════════════════════════════════════════════════════════╝");

    return PLUGIN_HANDLED;
}

@SrvCmd_LimitParams() {
    enum {Arg_sLimitTypeName = 1}

    new sLimitTypeName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    read_argv(Arg_sLimitTypeName, sLimitTypeName, charsmax(sLimitTypeName));

    new T_LimitType:iLimitType = LimitType_Find(sLimitTypeName);
    if (iLimitType == Invalid_LimitType) {
        server_print("Limit type '%s' not found.", sLimitTypeName);
        return PLUGIN_HANDLED;
    }

    new Array:aLimitTypeParams = LimitType_GetParams(iLimitType);
    PrintConfigParams(aLimitTypeParams);
    ArrayDestroy(aLimitTypeParams);

    return PLUGIN_HANDLED;
}

static PrintConfigParams(const Array:aParams) {
    server_print("╔═════╤══════════════════════════════════╤══════════╤══════════╗");
    server_print("║  #  │ Param name                       │ Type     │ Required ║");
    server_print("╟─────┼──────────────────────────────────┼──────────┼──────────╢");

    ArrayForeachArray2 (aParams: i => Param[S_CfgParam]) {
        server_print("║ %3d │ %-32s │ %-8s │ %-8s ║",
            i + 1, Param[CfgParam_Name],
            VIPM_PARAM_TYPE_NAMES[Param[CfgParam_Type]],
            Param[CfgParam_Required] ? "Yes" : "No"
        );
    }
    
    server_print("╟─────┴──────────────────────────────────┴──────────┴──────────╢");
    server_print("║ Total: %-5d                                                 ║", ArraySizeSafe(aParams));
    server_print("╚══════════════════════════════════════════════════════════════╝");
}
