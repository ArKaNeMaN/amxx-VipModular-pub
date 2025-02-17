#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_Armor_Register() {
    IC_ItemType_SimpleRegister(
        .name = "Armor",
        .onRead = "@OnArmorRead",
        .onGive = "@OnArmorGive"
    );
}

@OnArmorRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Armor", JSONNumber)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Armor` required for `Armor` item.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Armor", json_object_get_number(instanceJson, "Armor"));

    if (json_object_has_value(instanceJson, "MaxArmor", JSONNumber)) {
        TrieSetCell(p, "MaxArmor", json_object_get_number(instanceJson, "MaxArmor"));
    }

    if (json_object_has_value(instanceJson, "SetArmor", JSONBoolean)) {
        TrieSetCell(p, "SetArmor", json_object_get_bool(instanceJson, "SetArmor"));
    }

    if (json_object_has_value(instanceJson, "Helmet", JSONBoolean)) {
        TrieSetCell(p, "Helmet", json_object_get_bool(instanceJson, "Helmet"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnArmorGive(const playerIndex, const Trie:p) {
    if (VipM_Params_GetBool(p, "SetArmor", false)) {
        rg_set_user_armor(playerIndex, VipM_Params_GetInt(p, "Armor"), VipM_Params_GetBool(p, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR);
    } else {
        new iSetArmor = min(rg_get_user_armor(playerIndex) + VipM_Params_GetInt(p, "Armor"), VipM_Params_GetInt(p, "MaxArmor", 100));

        rg_set_user_armor(playerIndex, iSetArmor, VipM_Params_GetBool(p, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR);
    }
}
