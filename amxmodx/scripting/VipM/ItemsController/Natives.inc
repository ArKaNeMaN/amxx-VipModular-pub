#include amxmodx
#include json
#include "VipM/Utils"

#define RegisterNative(%1) \
    register_native(fmt(__NATIVE_NAME_TPL, %1), fmt(__NATIVE_FUNC_TPL, %1))

new const __NATIVE_NAME_TPL[] = "VipM_IC_%s";
new const __NATIVE_FUNC_TPL[] = "@_%s";

enum {
    ERROR_TYPE_NOT_FOUND,
    ERROR_ITEM_NOT_FOUND,
}

#define NATIVE_CHECK_ITEM(%1) CompositeMacros( \
    if (!ITEM_EXISTS(%1)) { \
        log_error(ERROR_ITEM_NOT_FOUND, "[ERROR] Item #%d not found.", %1); \
    } \
)

#define NATIVE_CHECK_TYPE(%1) CompositeMacros( \
    if (!TYPE_EXISTS(%1)) { \
        log_error(ERROR_TYPE_NOT_FOUND, "[ERROR] Item type '%s' not found.", %1);\
    } \
)

public plugin_natives() {
    RegisterNative("RegisterType");
    RegisterNative("RegisterTypeEvent");
    RegisterNative("JsonGetItem");
    RegisterNative("GiveItem");
}

@_RegisterType(const PluginId) {
    enum {Arg_Type = 1}

    new Type[S_ItemType];
    get_string(Arg_Type, Type[ItemType_Name], charsmax(Type[ItemType_Name]));

    Type[ItemType_PluginId] = PluginId;
    Type[ItemType_Events] = Invalid_Trie;

    return ADD_TYPE(Type);
}

@_RegisterTypeEvent(const PluginId) {
    enum {Arg_Type = 1, Arg_Event, Arg_Func}
    
    new TypeName[32], Type[S_ItemType];
    get_string(Arg_Type, TypeName, charsmax(TypeName));

    NATIVE_CHECK_TYPE(TypeName);
    GET_TYPE(TypeName, Type);

    new E_ItemTypeEvent:Event = E_ItemTypeEvent:get_param(Arg_Event);

    new FuncName[64];
    get_string(Arg_Func, FuncName, charsmax(FuncName));

    new FwdId = -1;
    switch (Event) {
        case ItemType_OnRead: // VipM_FwdReturn:(const JSON:jItem, Trie:Params)
            FwdId = CreateOneForward(PluginId, FuncName, FP_CELL, FP_CELL);

        case ItemType_OnGive: // VipM_FwdReturn:(const UserId, const Trie:Params)
            FwdId = CreateOneForward(PluginId, FuncName, FP_CELL, FP_CELL);
    }

    if (FwdId < 0) {
        return false;
    }

    SET_TYPE_EVENT(Type, Event, FwdId);
    SET_TYPE(Type);
    
    return FwdId >= 0;
}

@_JsonGetItem(){
    enum {Arg_jItem = 1}

    new JSON:jItem = JSON:get_param_byref(Arg_jItem);
    new ItemId = Cfg_JsonGetItem(jItem);
    set_param_byref(Arg_jItem, _:Invalid_JSON);
    return ItemId;
}

bool:@_GiveItem(){
    enum {Arg_UserId = 1, Arg_ItemId}

    new UserId = get_param(Arg_UserId);
    new ItemId = get_param(Arg_ItemId);

    NATIVE_CHECK_ITEM(ItemId);
    new Item[S_Item];
    GET_ITEM(ItemId, Item);

    new Type[S_ItemType];
    GET_ITEM_TYPE(Item, Type);

    FwdRetInit(VIPM_CONTINUE);
    FwdExecP(GiveItem, [UserId, Item[Item_Params]]);
    if (FwdRet() == VIPM_STOP) {
        return false;
    }

    new Ret = VIPM_CONTINUE;
    EMIT_TYPE_EVENT(Type, ItemType_OnGive, Ret, UserId, Item[Item_Params])
    if (Ret == VIPM_STOP) {
        return false;
    }

    return true;
}
