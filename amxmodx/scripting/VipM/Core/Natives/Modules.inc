#include <amxmodx>
#include "VipM/DebugMode"

Natives_Modules_Init(){
    RegisterNative("Modules_Register");
    RegisterNative("Modules_AddParams");
    RegisterNative("Modules_RegisterEvent");
    RegisterNative("Modules_IsActive");
    RegisterNative("Modules_GetParams");
}

@_Modules_Register(const PluginId){
    enum {Arg_Module = 1, Arg_Once}

    new Module[S_Module];

    get_string(Arg_Module, Module[Module_Name], charsmax(Module[Module_Name]));
    Module[Module_PluginId] = PluginId;
    Module[Module_Enabled] = false;
    Module[Module_Once] = bool:get_param(Arg_Once);

    Module[Module_Params] = Invalid_Array;
    Module[Module_Events] = Invalid_Trie;

    ArrayMapPushArray(Modules, Module, Module[Module_Name]);
}

@_Modules_AddParams(const PluginId, const ParamsCount){
    enum {Arg_Module = 1, Arg_Params}
    
    new ModuleName[32], Module[S_Module];
    get_string(Arg_Module, ModuleName, charsmax(ModuleName));

    NATIVE_CHECK_MODULE(ModuleName)
    GET_MODULE(ModuleName, Module);
    new bool:bSave = (Module[Module_Params] == Invalid_Array);

    Dbg_PrintServer("Module_AddParams(%s): bSave = %s", ModuleName, bSave ? "+" : "-");
    
    Module[Module_Params] = Native_GetParamsList(Arg_Params - 1, ParamsCount, Module[Module_Params]);
    
    Dbg_PrintServer(":---");
    
    if (bSave) {
        SET_MODULE(Module);
    }
}

bool:@_Modules_RegisterEvent(const PluginId){
    enum {Arg_Module = 1, Arg_Event, Arg_Func}
    
    new ModuleName[32], Module[S_Module];
    get_string(Arg_Module, ModuleName, charsmax(ModuleName));

    NATIVE_CHECK_MODULE(ModuleName)
    GET_MODULE(ModuleName, Module);

    new E_ModuleEvent:Event = E_ModuleEvent:get_param(Arg_Event);

    new FuncName[64];
    get_string(Arg_Func, FuncName, charsmax(FuncName));

    new FwdId = -1;
    switch (Event) {
        case Module_OnRead: // (const JSON:jCfg, Trie:Params)
            FwdId = CreateOneForward(PluginId, FuncName, FP_CELL, FP_CELL);

        case Module_OnActivated: // ()
            FwdId = CreateOneForward(PluginId, FuncName);

        case Module_OnCompareParams: // (Trie:MainParams, const Trie:NewParams)
            FwdId = CreateOneForward(PluginId, FuncName, FP_CELL, FP_CELL);
    }

    if (FwdId < 0) {
        return false;
    }

    SET_MODULE_EVENT(Module, Event, FwdId);
    SET_MODULE(Module);
    
    return FwdId >= 0;
}

@_Modules_IsActive(){
    enum {Arg_Module = 1}
    
    static ModuleName[32], Module[S_Module];
    get_string(Arg_Module, ModuleName, charsmax(ModuleName));

    NATIVE_CHECK_MODULE(ModuleName)
    GET_MODULE(ModuleName, Module);

    return Module[Module_Enabled];
}

Trie:@_Modules_GetParams(){
    enum {Arg_Module = 1, Arg_UserId}
    
    static ModuleName[32];
    get_string(Arg_Module, ModuleName, charsmax(ModuleName));
    NATIVE_CHECK_MODULE(ModuleName)

    new UserId = get_param(Arg_UserId);
    
    new Trie:Params = Invalid_Trie;
    if (
        !is_user_connected(UserId)
        || gUserVip[UserId] == Invalid_Trie
        || !TrieGetCell(gUserVip[UserId], ModuleName, Params)
    ) {
        return Invalid_Trie;
    }
    
    return Params;
}
