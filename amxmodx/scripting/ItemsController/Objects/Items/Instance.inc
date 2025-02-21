#if defined __ic__objects__items_instance__included
    #endinput
#endif
// Два кванта
#define __ic__objects__items_instance__included

#include <amxmodx>
#include <ItemsController>
#include "VipM/Forwards"
#include "VipM/Utils"
#include "VipM/JsonUtils"

#include "ItemsController/Objects/Items/Type"

enum _:S_ItemInstance {
    T_IC_ItemType:Item_Type,
    Trie:Item_Params,
}

static Array:ItemInstances = Invalid_Array;
static Stack:FreeItemInstanceHandlers = Invalid_Stack;
static ActiveItemInstancesCount = 0;

ItemInstance_Init() {
    CallOnce();

    ItemType_Init();

    ItemInstances = ArrayCreate(S_ItemInstance, 1);
    FreeItemInstanceHandlers = CreateStack();
    ActiveItemInstancesCount = 0;

    Forwards_RegAndCall("IC_Item_OnInited", ET_IGNORE);
}

T_IC_Item:ItemInstance_Construct(const T_IC_ItemType:type, const Trie:params) {
    if (ItemInstances == Invalid_Array) {
        abort(AMX_ERR_GENERAL, "Attempt to create item instance before items controller init.");
        return Invalid_IC_Item;
    }

    new instanceObject[S_ItemInstance];
    instanceObject[Item_Type] = type;
    instanceObject[Item_Params] = params;
    
    ActiveItemInstancesCount++;
    if (IsStackEmpty(FreeItemInstanceHandlers)) {
        return T_IC_Item:ArrayPushArray(ItemInstances, instanceObject);
    } else {
        new freeInstanceHandler;
        PopStackCell(FreeItemInstanceHandlers, freeInstanceHandler);

        ArraySetArray(ItemInstances, freeInstanceHandler, instanceObject);
        return T_IC_Item:freeInstanceHandler;
    }
}

bool:ItemInstance_Get(const T_IC_Item:instance, instanceObject[S_ItemInstance], const bool:orFail = true) {
    if (_:instance < 0 || _:instance >= ArraySize(ItemInstances)) {
        if (orFail) {
            abort(AMX_ERR_NOTFOUND, "Invalid item instance handler (%d, out of bounds).", instance);
        }
        return false;
    }

    ArrayGetArray(ItemInstances, _:instance, instanceObject);

    if (instanceObject[Item_Type] == Invalid_IC_ItemType) {
        if (orFail) {
            abort(AMX_ERR_NOTFOUND, "Invalid item instance handler (%d, deleted).", instance);
        }
        return false;
    }

    return true;
}

bool:ItemInstance_Give(const playerIndex, const T_IC_Item:instance) {
    new instanceObject[S_ItemInstance];
    ItemInstance_Get(instance, instanceObject);

    return ItemType_Give(playerIndex, instanceObject[Item_Type], instanceObject[Item_Params]);
}

T_IC_Item:ItemInstance_Free(&T_IC_Item:instance, const bool:orFail = false) {
    new instanceObject[S_ItemInstance];
    if (!ItemInstance_Get(instance, instanceObject, .orFail = orFail)) {
        return instance = Invalid_IC_Item;
    }

    instanceObject[Item_Type] = Invalid_IC_ItemType;
    TrieDestroy(instanceObject[Item_Params]); // TODO: Надо что-то придумать на случай вложенных динамических штук

    ArraySetArray(ItemInstances, _:instance, instanceObject);
    PushStackCell(FreeItemInstanceHandlers, instance);

    ActiveItemInstancesCount--;
    return instance = Invalid_IC_Item;
}

T_IC_Item:ItemInstance_ReadFromJsonObject(const JSON:instanceJson) {
    if (!json_is_object(instanceJson)) {
        Json_ErrorForFile(instanceJson, "Invalid item object (json value is not an object).");
        return Invalid_IC_Item;
    }

    new typeName[IC_ITEM_TYPE_NAME_MAX_LEN];
    if (json_object_has_value(instanceJson, "Item", JSONString)) {
        json_object_get_string(instanceJson, "Item", typeName, charsmax(typeName));
    } else if (json_object_has_value(instanceJson, "Type", JSONString)) {
        Json_LogForFile(instanceJson, "WARNING", "Item object with field `Type` is deprecated, use `Item` field instead.");
        json_object_get_string(instanceJson, "Type", typeName, charsmax(typeName));
    } else {
        Json_ErrorForFile(instanceJson, "Invalid item object (must contains `Item` field).");
        return Invalid_IC_Item;
    }

    trim(typeName);
    if (!typeName[0]) {
        Json_ErrorForFile(instanceJson, "Item type is empty.");
        return Invalid_IC_Item;
    }

    new T_IC_ItemType:type = ItemType_Find(typeName);
    if (type == Invalid_IC_ItemType) {
        Json_ErrorForFile(instanceJson, "Item type '%s' not found.", typeName);
        return Invalid_IC_Item;
    }

    new Trie:params = TrieCreate();
    if (!ItemType_ReadParamsFromJsonObject(type, instanceJson, params)) {
        TrieDestroy(params);
        return Invalid_IC_Item;
    }

    return ItemInstance_Construct(type, params);
}

Array:ItemInstance_ReadArrayFromJsonValue(const JSON:instancesJson, &Array:array = Invalid_Array) {
    if (json_is_array(instancesJson)) {
        if (array == Invalid_Array) {
            array = ArrayCreate(1, json_array_get_count(instancesJson));
        }

        for (new i = 0, ii = json_array_get_count(instancesJson); i < ii; i++) {
            new JSON:instanceJson = json_array_get_value(instancesJson, i);

            new T_IC_Item:instance = ItemInstance_ReadFromJsonObject(instanceJson);
            if (instance != Invalid_IC_Item) {
                ArrayPushCell(array, instance);
            }

            json_free(instanceJson);
        }
    } else if (json_is_object(instancesJson)) {
        if (array == Invalid_Array) {
            array = ArrayCreate(1, 1);
        }

        new T_IC_Item:instance = ItemInstance_ReadFromJsonObject(instancesJson);
        if (instance != Invalid_IC_Item) {
            ArrayPushCell(array, instance);
        }
    } else {
        Json_ErrorForFile(instancesJson, "Json value must be an array or an object.");
    }
    
    return array;
}
