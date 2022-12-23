#include <amxmodx>
#include <json>
#include <VipModular>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM-I] Limits";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "[VipModular-Item] Items using limtis.";

public VipM_IC_OnInitTypes() {
    RegisterPluginByVars();

    VipM_IC_RegisterType("If");
    VipM_IC_RegisterTypeEvent("If", ItemType_OnRead, "@OnIfRead");
    VipM_IC_RegisterTypeEvent("If", ItemType_OnGive, "@OnIfGive");
}

@OnIfRead(const JSON:jItem, const Trie:tParams) {
    TrieDeleteKey(tParams, "Name");

    if (!json_object_has_value(jItem, "Items")) {
        Json_LogForFile(jItem, "ERROR", "Param `Items` required for item `If`.");
        return VIPM_STOP;
    }

    if (!json_object_has_value(jItem, "Limits")) {
        Json_LogForFile(jItem, "ERROR", "Limits `Items` required for item `If`.");
        return VIPM_STOP;
    }
    
    TrieSetCell(tParams, "Items", VipM_IC_JsonGetItems(json_object_get_value(jItem, "Items")));
    TrieSetCell(tParams, "Limits", VipM_Limits_ReadListFromJson(json_object_get_value(jItem, "Limits")));

    return VIPM_CONTINUE;
}

@OnIfGive(const UserId, const Trie:tParams) {
    if (VipM_Params_ExecuteLimitsList(tParams, "Limits", UserId, Limit_Exec_AND)) {
        VipM_IC_GiveItems(UserId, VipM_Params_GetArr(tParams, "Items"));
        return VIPM_CONTINUE;
    }
    return VIPM_STOP;
}
