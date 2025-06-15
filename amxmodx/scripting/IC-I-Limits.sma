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
public stock const PluginDescription[] = "[ItemsController-Item] Items using limits.";

public IC_ItemType_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "If",
        .onGive = "@OnIfGive"
    );
    IC_ItemType_AddParams(type,
        "Limits", "VipM-Limits", true,
        "Items", "IC-Items", false,
        "ElseItems", "IC-Items", false
    );
}

@OnIfGive(const playerIndex, const Trie:p) {
    if (!PCGet_VipmLimitsCheck(p, "Limits", playerIndex, Limit_Exec_AND)) {
        return PCGet_IcItemsGive(p, "ElseItems", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
    }

    return PCGet_IcItemsGive(p, "Items", playerIndex) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}
