#include amxmodx
#include json

new Trie:__JsonFileCache = Invalid_Trie;

Cfg_JsonGetItem(&JSON:jItem){
    if (__JsonFileCache == Invalid_Trie) {
        __JsonFileCache = TrieCreate();
    }

    // Use `Json_IsRef` instead deprecated `JSON_GET_FILE_OR_OBJECT`
    jItem = JSON_GET_FILE_OR_OBJECT(jItem);
    if (jItem == Invalid_JSON) {
        return -1;
    }

    new NewItemIndex = -1;
    if (
        HasLastJsonObjectFile()
        && TrieGetCell(__JsonFileCache, GetLastJsonObjectFile(), NewItemIndex)
        && NewItemIndex >= 0
    ) {
        json_free(jItem);
        return NewItemIndex;
    }

    SaveLastJsonObjectFile(__FILE_NAME);

    if (
        !json_is_object(jItem)
        || !json_object_has_value(jItem, "Type", JSONString)
    ) {
        log_amx("[WARNING] Invalid item format.");
        return -1;
    }
    
    new TypeName[32];
    json_object_get_string(jItem, "Type", TypeName, charsmax(TypeName));
    json_object_remove(jItem, "Type");

    if (!TYPE_EXISTS(TypeName)) {
        log_amx("[WARNING] Item type `%s` not found.", TypeName);
        return -1;
    }

    new Item[S_Item];
    Item[Item_Type] = GET_TYPE_ID(TypeName);

    new Type[S_ItemType];
    GET_TYPE_BY_ID(Item[Item_Type], Type);

    Item[Item_Params] = TrieCreate();
    
    if (json_object_has_value(jItem, "Name", JSONString)) {
        new ItemName[32];
        json_object_get_string(jItem, "Name", ItemName, charsmax(ItemName));
        TrieSetString(Item[Item_Params], "Name", ItemName);
    }

    FwdRetInit(VIPM_CONTINUE);
    FwdExecP(ReadItem, [jItem, Item[Item_Params]]);
    if (FwdRet() == VIPM_STOP) {
        TrieDestroySafe(Item[Item_Params]);
        return -1;
    }

    new Ret = VIPM_CONTINUE;
    EMIT_TYPE_EVENT(Type, ItemType_OnRead, Ret, jItem, Item[Item_Params])
    if (Ret == VIPM_STOP) {
        TrieDestroySafe(Item[Item_Params]);
        return -1;
    }

    if (TrieSizeSafe(Item[Item_Params]) < 1) {
        TrieDestroySafe(Item[Item_Params]);
    }

    NewItemIndex = ADD_ITEM(Item);

    if (__FILE_NAME[0]) {
        TrieSetCell(__JsonFileCache, __FILE_NAME, NewItemIndex);
    }

    json_free(jItem);

    return NewItemIndex;
}
