#include amxmodx
#include json
#include "VipM/JsonUtils"

// TODO: Переписать на хендлеры

static Trie:g_ItemsCache = Invalid_Trie;

Cfg_JsonGetItem(&JSON:jItem) {
    TrieCreateIfNotCreated(g_ItemsCache);
    new iItem = -1;

    new sJsonRef[PLATFORM_MAX_PATH];
    if (Json_IsRef(jItem, sJsonRef, charsmax(sJsonRef))) {
        if (TrieGetCell(g_ItemsCache, sJsonRef, iItem)) {
            Json_FreeEx(jItem);
            return iItem;
        }

        // jItem = Json_GetRefValue(jItem, .bFreeSrcIfRef=true);
    }

    if (jItem == Invalid_JSON) {
        return -1;
    }
    
    new sItemTypeName[32];
    if (
        !json_is_object(jItem)
        || !Json_Object_GetItemTypeAndRemove(jItem, sItemTypeName, charsmax(sItemTypeName))
    ) {
        Json_LogForFile(jItem, "WARNING", "Invalid item format.");
        Json_FreeEx(jItem);
        return -1;
    }

    if (!TYPE_EXISTS(sItemTypeName)) {
        Json_LogForFile(jItem, "WARNING", "Item type `%s` not found.", sItemTypeName);
        Json_FreeEx(jItem);
        return -1;
    }

    new Item[S_Item];
    Item[Item_Type] = GET_TYPE_ID(sItemTypeName);
    Item[Item_Params] = TrieCreate();
    
    if (json_object_has_value(jItem, "Name", JSONString)) {
        new sItemName[32];
        json_object_get_string(jItem, "Name", sItemName, charsmax(sItemName));
        TrieSetString(Item[Item_Params], "Name", sItemName);
    }

    Forwards_DefaultReturn(VIPM_CONTINUE);
    if (Forwards_CallP("ReadItem", jItem, Item[Item_Params]) == VIPM_STOP) {
        TrieDestroySafe(Item[Item_Params]);
        Json_FreeEx(jItem);
        return -1;
    }

    new ItemType[S_ItemType];
    GET_TYPE_BY_ID(Item[Item_Type], ItemType);

    new iRet = VIPM_CONTINUE;
    EMIT_TYPE_EVENT(ItemType, ItemType_OnRead, iRet, jItem, Item[Item_Params])
    Json_FreeEx(jItem);

    if (iRet == VIPM_STOP) {
        TrieDestroySafe(Item[Item_Params]);
        return -1;
    }

    // А надо ли оно?
    if (TrieSizeSafe(Item[Item_Params]) < 1) {
        TrieDestroySafe(Item[Item_Params]);
    }

    iItem = ADD_ITEM(Item);

    if (sJsonRef[0]) {
        TrieSetCell(g_ItemsCache, sJsonRef, iItem);
    }

    return iItem;
}

// TODO: `Type` для совместимости, надо будет когда-нить убрать)
static Json_Object_GetItemTypeAndRemove(const JSON:jItem, sItemType[], const iItemTypeLen) {
    new iRet = 0;
    if (json_object_has_value(jItem, "Type", JSONString)) {
        iRet = json_object_get_string(jItem, "Type", sItemType, iItemTypeLen);
        json_object_remove(jItem, "Type");
    } else if (json_object_has_value(jItem, "ItemType", JSONString)) {
        iRet = json_object_get_string(jItem, "ItemType", sItemType, iItemTypeLen);
        json_object_remove(jItem, "ItemType");
    }
    return iRet;
}
