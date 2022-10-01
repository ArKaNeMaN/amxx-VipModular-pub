#include <amxmodx>
#include <VipModular>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][L] Logic";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";

public VipM_OnInitModules(){
    RegisterPluginByVars();

    VipM_Limits_RegisterType("Logic-OR", false, false);
    VipM_Limits_AddTypeParams("Logic-OR",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-OR", Limit_OnCheck, "@OnOrCheck");

    VipM_Limits_RegisterType("Logic-XOR", false, false);
    VipM_Limits_AddTypeParams("Logic-XOR",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-XOR", Limit_OnCheck, "@OnXorCheck");

    VipM_Limits_RegisterType("Logic-AND", false, false);
    VipM_Limits_AddTypeParams("Logic-AND",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-AND", Limit_OnCheck, "@OnAndCheck");

    VipM_Limits_RegisterType("Logic-NOT", false, false);
    VipM_Limits_AddTypeParams("Logic-NOT",
        "Limits", ptLimits, true
    );
    VipM_Limits_RegisterTypeEvent("Logic-NOT", Limit_OnCheck, "@OnNotCheck");
}

@OnOrCheck(const Trie:Params, const UserId){
    return VipM_Params_ExecuteLimitsList(Params, "Limits", UserId, Limit_Exec_OR);
}

@OnXorCheck(const Trie:Params, const UserId){
    return VipM_Params_ExecuteLimitsList(Params, "Limits", UserId, Limit_Exec_XOR);
}

@OnAndCheck(const Trie:Params, const UserId){
    return VipM_Params_ExecuteLimitsList(Params, "Limits", UserId, Limit_Exec_AND);
}

@OnNotCheck(const Trie:Params, const UserId){
    return !VipM_Params_ExecuteLimitsList(Params, "Limits", UserId, Limit_Exec_AND);
}
