#if defined _vipmodular_src_SrvCmds_included
    #endinput
#endif
#define _vipmodular_src_SrvCmds_included

#include <amxmodx>
#include "VipM/ArrayTrieUtils"
#include "VipM/ArrayMap"

#include "VipM/Core/Modules/Main"
#include "VipM/Core/Limits/Main"
#include "VipM/Core/Structs"

SrvCmds_Init(){
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
    server_print("║ │ │ ├─Modules count: %d", ArrayMapSize(Modules));
    server_print("║ │ │ └─Module units count: %d", ArraySizeSafe(ModuleUnits));
    server_print("║ │ ├─Limits:");
    server_print("║ │ │ ├─Types count: %d", ArrayMapSize(Limits));
    server_print("║ │ │ └─Limit units count: %d", ArraySizeSafe(LimitUnits));
    server_print("║ │ ├─Vips:");
    server_print("║ │ │ └─Vips count: %d", ArraySizeSafe(Vips));
    server_print("║ │ └─Compiled by AmxModX v%s:", AMXX_VERSION_STR);
    server_print("║ │");
    server_print("║ └─GitHub repo: https://github.com/ArKaNeMaN/amxx-VipModular-pub");
    server_print("╙─────────");
}

@SrvCmd_Modules(){
    server_print("╔═════╤══════════════════════════════════╤════════╗");
    server_print("║  #  │ Module name                      │ Status ║");
    server_print("╟─────┼──────────────────────────────────┼────────╢");
    ArrayMapForeachArray2 (Modules: i => Module[S_Module]) {
        server_print("║ %03d │ %-32s │ %-6s ║", i + 1, Module[Module_Name], Module[Module_Enabled] ? "On" : "Off");
    }
    server_print("╟─────┴──────────────────────────────────┴────────╢");
    server_print("║ Total: %-5d                                    ║", ArrayMapSize(Modules));
    server_print("╚═════════════════════════════════════════════════╝");
}

@SrvCmd_ModuleParams(){
    new Module[S_Module], ModuleName[32];
    read_argv(1, ModuleName, charsmax(ModuleName));

    if (!MODULE_EXISTS(ModuleName)) {
        server_print("Module `%s` not found.", ModuleName);
        return;
    }

    GET_MODULE(ModuleName, Module);
    
    PrintConfigParams(Module[Module_Params]);
}

@SrvCmd_Limits(){
    server_print("╔═════╤══════════════════════════════════╤════════╤════════╤════════════╗");
    server_print("║  #  │ Limit name                       │ Params │ Static │ For player ║");
    server_print("╟─────┼──────────────────────────────────┼────────┼────────┼────────────╢");
    ArrayMapForeachArray2 (Limits: i => Limit[S_Limit]) {
        server_print("║ %03d │ %-32s │ %-6d │ %-6s │ %-10s ║",
            i + 1, Limit[Limit_Name],
            ArraySizeSafe(Limit[Limit_Params]),
            Limit[Limit_Static] ? "Yes" : "No",
            Limit[Limit_ForPlayer] ? "Yes" : "No"
        );
    }
    server_print("╟─────┴──────────────────────────────────┴────────┴────────┴────────────╢");
    server_print("║ Total: %-5d                                                          ║", ArrayMapSize(Limits));
    server_print("╚═══════════════════════════════════════════════════════════════════════╝");
}

@SrvCmd_LimitParams(){
    new LimitType[S_Limit], LimitName[32];
    read_argv(1, LimitName, charsmax(LimitName));

    if (!LIMIT_TYPE_EXISTS(LimitName)) {
        server_print("Limit `%s` not found.", LimitName);
        return;
    }

    LIMIT_TYPE_GET(LimitName, LimitType);
    
    PrintConfigParams(LimitType[Limit_Params]);
}

static PrintConfigParams(const Array:Params) {
    server_print("╔═════╤══════════════════════════════════╤══════════╤══════════╗");
    server_print("║  #  │ Param name                       │ Type     │ Required ║");
    server_print("╟─────┼──────────────────────────────────┼──────────┼──────────╢");
    ArrayForeachArray2 (Params: i => Param[S_CfgParam]) {
        server_print("║ %3d │ %-32s │ %-8s │ %-8s ║",
            i + 1, Param[CfgParam_Name],
            VIPM_PARAM_TYPE_NAMES[Param[CfgParam_Type]],
            Param[CfgParam_Required] ? "Yes" : "No"
        );
    }
    server_print("╟─────┴──────────────────────────────────┴──────────┴──────────╢");
    server_print("║ Total: %-5d                                                 ║", ArraySizeSafe(Params));
    server_print("╚══════════════════════════════════════════════════════════════╝");
}
