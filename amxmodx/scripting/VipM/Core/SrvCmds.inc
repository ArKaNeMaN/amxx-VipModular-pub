#if defined _vipmodular_src_SrvCmds_included
    #endinput
#endif
#define _vipmodular_src_SrvCmds_included

SrvCmds_Init(){
    register_srvcmd("vipm_modules", "@SrvCmd_Modules");
    register_srvcmd("vipm_module_params", "@SrvCmd_ModuleParams");

    register_srvcmd("vipm_limits", "@SrvCmd_Limits");
    register_srvcmd("vipm_limit_params", "@SrvCmd_LimitParams");
}

@SrvCmd_Modules(){
    PrintModulesData(Modules);
}

@SrvCmd_ModuleParams(){
    new Module[S_Module], ModuleName[32];
    read_argv(1, ModuleName, charsmax(ModuleName));

    if(!MODULE_EXISTS(ModuleName)){
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

    if(!LIMIT_TYPE_EXISTS(LimitName)){
        server_print("Limit `%s` not found.", LimitName);
        return;
    }

    LIMIT_TYPE_GET(LimitName, LimitType);
    
    PrintConfigParams(LimitType[Limit_Params]);
}
