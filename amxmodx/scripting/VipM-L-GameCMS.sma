#include <amxmodx>
#include <VipModular>
#include <gamecms5>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][L] GameCMS";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "[VipModular] Access by GameCMS services";

public VipM_OnInitModules(){
    RegisterPluginByVars();

    VipM_Limits_RegisterType("GCMS-Service", true, false);
    VipM_Limits_AddTypeParams("GCMS-Service",
        "Service", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("GCMS-Service", Limit_OnCheck, "@OnServiceCheck");

    VipM_Limits_RegisterType("GCMS-Member", false, false);
    VipM_Limits_AddTypeParams("GCMS-Member",
        "GroupName", ptString, false
    );
    VipM_Limits_RegisterTypeEvent("GCMS-Member", Limit_OnCheck, "@OnMemberCheck");
}

@OnMemberCheck(const Trie:Params, const UserId){
    new sGroupName[64];
    new iGroupId = cmsapi_get_user_group(UserId, sGroupName, charsmax(sGroupName));
    if (!iGroupId) {
        return false;
    }

    new spGroupName[64];
    VipM_Params_GetStr(Params, "Group", sGroupName, charsmax(sGroupName));
    if (!sGroupName[0]) {
        return true;
    }

    return equali(sGroupName, spGroupName);
}

@OnServiceCheck(const Trie:Params, const UserId){
    new Service[32];
    TrieGetString(Params, "Service", Service, charsmax(Service));

    return (cmsapi_get_user_services(UserId, "", Service) != Invalid_Array);
}
