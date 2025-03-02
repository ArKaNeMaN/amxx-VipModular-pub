#include <amxmodx>
#include <reapi>
#include <hamsandwich>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_Health_Register() {
    IC_ItemType_SimpleRegister(
        .name = "Health",
        .onRead = "@OnHealthRead",
        .onGive = "@OnHealthGive"
    );
}

@OnHealthRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Health", JSONNumber)) {
        PCJson_LogForFile(instanceJson, "ERROR", "Param `Health` required for `Health` item.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Health", json_object_get_real(instanceJson, "Health"));

    if (json_object_has_value(instanceJson, "MaxHealth", JSONNumber)) {
        TrieSetCell(p, "MaxHealth", json_object_get_real(instanceJson, "MaxHealth"));
    }

    if (json_object_has_value(instanceJson, "SetHealth", JSONBoolean)) {
        TrieSetCell(p, "SetHealth", json_object_get_bool(instanceJson, "SetHealth"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnHealthGive(const playerIndex, const Trie:p) {
    if (VipM_Params_GetBool(p, "SetHealth", false)) {
        set_entvar(playerIndex, var_health, VipM_Params_GetFloat(p, "Health"));
    } else {
        new Float:fHealth = Float:get_entvar(playerIndex, var_health);
        new Float:fMaxHealth = VipM_Params_GetFloat(p, "MaxHealth", 100.0);
        new Float:fAddHealth = floatclamp(VipM_Params_GetFloat(p, "Health"), 0.0, floatmax(0.0, fMaxHealth - fHealth));
        new Float:fMaxHealthCurrent = Float:get_entvar(playerIndex, var_max_health);
        new bool:bNeedOverrideMaxHealth = (fHealth < fMaxHealth && fMaxHealthCurrent < fMaxHealth);

        if (bNeedOverrideMaxHealth) {
            set_entvar(playerIndex, var_max_health, fMaxHealth);
        }

        ExecuteHamB(Ham_TakeHealth, playerIndex, fAddHealth, DMG_GENERIC);

        if (bNeedOverrideMaxHealth) {
            set_entvar(playerIndex, var_max_health, fMaxHealthCurrent);
        }
    }
}
