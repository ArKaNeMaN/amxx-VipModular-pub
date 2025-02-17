#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_Random_Register() {
    IC_ItemType_SimpleRegister(
        .name = "Random",
        .onRead = "@OnRandomRead",
        .onGive = "@OnRandomGive"
    );
}

@OnRandomRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    // TODO: Сделать как-то разные шансы
    if (!json_object_has_value(instanceJson, "Items")) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Items` required for item `Random`.");
        return IC_RET_READ_FAIL;
    }
    
    new Array:aItems = Json_Object_IC_GetItems(instanceJson, "Items");
    if (ArraySizeSafe(aItems) <= 1) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Items` must have >1 items.");
        ArrayDestroy(aItems);
        return IC_RET_READ_FAIL;
    }

    TrieSetCell(p, "Items", aItems);

    return IC_RET_READ_SUCCESS;
}

@OnRandomGive(const playerIndex, const Trie:p) {
    new Array:aItems = VipM_Params_GetArr(p, "Items");
    new iRandomIndex = random_num(0, ArraySizeSafe(aItems) - 1);
    new VipM_IC_T_Item:iRandomItem = ArrayGetCell(aItems, iRandomIndex);

    return IC_Item_Give(playerIndex, iRandomItem) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}
