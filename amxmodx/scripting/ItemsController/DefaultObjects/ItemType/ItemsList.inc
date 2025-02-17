#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_ItemsList_Register() {
    IC_ItemType_SimpleRegister(
        .name = "ItemsList",
        .onRead = "@OnItemsListRead",
        .onGive = "@OnItemsListGive"
    );
}

@OnItemsListRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Items")) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Items` required for item `ItemsList`.");
        return IC_RET_READ_FAIL;
    }

    TrieSetCell(p, "Items", Json_Object_IC_GetItems(instanceJson, "Items"));

    return IC_RET_READ_SUCCESS;
}

@OnItemsListGive(const playerIndex, const Trie:p) {
    new Array:aItems = VipM_Params_GetCell(p, "Items", Invalid_Array);

    return IC_Item_GiveArray(playerIndex, aItems) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}
