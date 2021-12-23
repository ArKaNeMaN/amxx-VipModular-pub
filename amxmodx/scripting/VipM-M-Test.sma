#include <amxmodx>
#include <json>
#include <VipModular>

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][M] Test";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Vip modular`s testing module";

#define LOG(%1,%2) \
    log_amx("[%s] %s: %s", %1, MODULE_NAME, fmt(%2))

new const MODULE_NAME[] = "TestModule";

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);

    VipM_Modules_Register(MODULE_NAME, false);
    VipM_SetModuleParams(MODULE_NAME,
        "Num", ptInteger, true
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@Event_Activated");
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnRead, "@Event_Read");
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnCompareParams, "@Event_CompareParams");

    register_srvcmd("vipm_test", "@SrvCmd_Test");
    register_clcmd("vipm_cl_test", "@Cmd_ClTest");
}

@Event_Activated(){
    LOG("TEST", "Module_OnActivated");
}

bool:@Event_Read(const JSON:jCfg, Trie:Params){
    LOG("TEST", "Module_OnRead:");

    new Num = 0;
    TrieGetCell(Params, "Num", Num);
    LOG("TEST", "  Params['Num'] = %d", Num);
    LOG("TEST", "  jCfg['Num'] = %d", json_object_get_number(jCfg, "Num"));

    return true;
}

Trie:@Event_CompareParams(const Trie:MainParams, const Trie:NewParams){
    new Num1 = 0, Num2 = 0;
    TrieGetCell(MainParams, "Num", Num1);
    TrieGetCell(NewParams, "Num", Num2);

    new Trie:New = TrieCreate();
    TrieSetCell(New, "Num", Num1 + Num2);

    LOG("TEST", "Event_CompareParams: Map #%d", New);
    return New;
}

@SrvCmd_Test(){
    LOG("CMD", VipM_Modules_IsActive(MODULE_NAME) ? "On" : "Off");
}

@Cmd_ClTest(const UserId){
    if(!VipM_Modules_IsActive(MODULE_NAME)){
        client_print(UserId, print_console, "Module `%s` not activated.", MODULE_NAME);
        return;
    }
    
    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    if(Params == Invalid_Trie){
        client_print(UserId, print_console, "No access.");
        return;
    }

    new Num = 0;
    TrieGetCell(Params, "Num", Num);
    client_print(UserId, print_console, "[%s] Num = %d", MODULE_NAME, Num);
}
