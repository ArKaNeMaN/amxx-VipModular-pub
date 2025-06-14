#include <amxmodx>
#include <json>
#include <VipModular>
#include <ItemsController>
#include <ParamsController>
#include "VipM/Utils"

public stock const PluginName[] = "[IC-I] Limits";
public stock const PluginVersion[] = IC_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://github.com/ArKaNeMaN/amxx-VipModular-pub";
public stock const PluginDescription[] = "[ItemsController-Item] Items using limtis.";

public IC_ItemType_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    IC_ItemType_SimpleRegister(
        .name = "If",
        .onRead = "@OnIfRead",
        .onGive = "@OnIfGive"
    );
}

@OnIfRead(const JSON:instanceJson, const Trie:p) {
    if (!json_object_has_value(instanceJson, "Items")) {
        PCJson_LogForFile(instanceJson, "ERROR", "Param `Items` required for item `If`.");
        return IC_RET_READ_FAIL;
    }

    if (!json_object_has_value(instanceJson, "Limits")) {
        PCJson_LogForFile(instanceJson, "ERROR", "Limits `Items` required for item `If`.");
        return IC_RET_READ_FAIL;
    }
    
    TrieSetCell(p, "Items", IC_Item_ReadArrayFromJson(json_object_get_value(instanceJson, "Items")));
    TrieSetCell(p, "ElseItems", IC_Item_ReadArrayFromJson(json_object_get_value(instanceJson, "ElseItems")));
    TrieSetCell(p, "Limits", VipM_Limits_ReadListFromJson(json_object_get_value(instanceJson, "Limits")));

    return IC_RET_READ_SUCCESS;
}

@OnIfGive(const playerIndex, const Trie:p) {
    if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND)) {
        return PCGet_IcItemsGive(p, "ElseItems", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
    }

    return PCGet_IcItemsGive(p, "Items", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}
