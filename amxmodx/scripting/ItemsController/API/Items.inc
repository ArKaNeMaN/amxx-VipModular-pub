#include <amxmodx>
#include <json>
#include <ItemsController>

#include "ItemsController/Objects/Items/Type"
#include "ItemsController/Objects/Items/Instance"

API_Items_Register() {
    register_native("IC_ItemType_Register", "@_ItemType_Register");
    register_native("IC_ItemType_SetEventListener", "@_ItemType_SetEventListener");

    register_native("IC_Item_ReadFromJson", "@_Item_ReadFromJson");
    register_native("IC_Item_ReadArrayFromJson", "@_Item_ReadArrayFromJson");
    register_native("IC_Item_Give", "@_Item_Give");
    register_native("IC_Item_Free", "@_Item_Free");
}

T_IC_ItemType:@_ItemType_Register() {
    enum {Arg_Name = 1}

    new name[IC_ITEM_TYPE_NAME_MAX_LEN];
    get_string(Arg_Name, name, charsmax(name));

    return ItemType_Construct(name);
}

@_ItemType_SetEventListener(const pluginIndex) {
    enum {Arg_Type = 1, Arg_Event, Arg_FuncName}

    new T_IC_ItemType:type = T_IC_ItemType:get_param(Arg_Type);
    new E_ItemTypeEvent:event = E_ItemTypeEvent:get_param(Arg_Event);
    new funcName[IC_EVENT_LISTENER_FUNCTION_NAME_MAX_LEN];
    get_string(Arg_FuncName, funcName, charsmax(funcName));

    new listener = ItemType_MakeEventListener(event, pluginIndex, funcName);
    ItemType_SetEventListener(type, event, listener);
}

T_IC_Item:@_Item_ReadFromJson() {
    enum {Arg_InstanceJson = 1}

    new JSON:isntanceJson = JSON:get_param(Arg_InstanceJson);

    return ItemInstance_ReadFromJsonObject(isntanceJson);
}


Array:@_Item_ReadArrayFromJson() {
    enum {Arg_InstancesJson = 1, Arg_Array}

    new JSON:isntancesJson = JSON:get_param(Arg_InstancesJson);
    new Array:array = Array:get_param_byref(Arg_Array);

    array = ItemInstance_ReadArrayFromJsonValue(isntancesJson, array);
    set_param_byref(Arg_Array, _:array);

    return array;
}


bool:@_Item_Give() {
    enum {Arg_PlayerIndex = 1, Arg_Item}

    new playerIndex = get_param(Arg_PlayerIndex);
    new T_IC_Item:item = T_IC_Item:get_param(Arg_Item);

    return ItemInstance_Give(playerIndex, item);
}


bool:@_Item_Free() {
    enum {Arg_Item = 1}

    new T_IC_Item:item = T_IC_Item:get_param(Arg_Item);

    return ItemInstance_Free(item);
}
