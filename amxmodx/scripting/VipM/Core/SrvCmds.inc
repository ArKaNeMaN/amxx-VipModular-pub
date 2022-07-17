#if defined _vipmodular_src_SrvCmds_included
    #endinput
#endif
#define _vipmodular_src_SrvCmds_included

#include <amxmodx>

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
    server_print("║ │ ├─GitHub: github.com/akraneman");
    server_print("║ │ ├─DevCS: dev-cs.ru/members/949/");
    server_print("║ │ ├─VK: vk.com/akraneman");
    server_print("║ │ └─TG: t.me/arkaneman");
    server_print("║ │");
    server_print("║ ├─Stats:");
    server_print("║ │ ├─Modules:");
    server_print("║ │ │ ├─Modules count: %d", ArrayMapSize(Modules));
    server_print("║ │ │ └─Module units count: %d", ArraySizeSafe(ModuleUnits));
    server_print("║ │ ├─Limits:");
    server_print("║ │ │ ├─Types count: %d", ArrayMapSize(Limits));
    server_print("║ │ │ └─Limit units count: %d", ArraySizeSafe(LimitUnits));
    server_print("║ │ └─Vips:");
    server_print("║ │   └─Vips count: %d", ArraySizeSafe(Vips));
    server_print("║ │");
    server_print("║ └─GitHub repo: https://github.com/ArKaNeMaN/amxx-VipModular-pub");
    server_print("╙─────────");
}

@SrvCmd_Modules(){
    PrintModulesData(Modules);
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
    PrintLimitsData(Limits);
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
