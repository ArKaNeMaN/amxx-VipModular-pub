#if defined __vipm_weaponmenu_objects_menuitem_included
    #endinput
#endif
#define __vipm_weaponmenu_objects_menuitem_included

#include <amxmodx>
#include <VipModular>
#include "VipM/JsonUtils"

enum MenuItemType {
    MenuItemType_Default = 0,
    MenuItemType_Text,
    MenuItemType_Blank,
}

enum _:S_MenuItem {
    MenuItemType:MenuItem_Type,

    MenuItem_Title[64],
    Array:MenuItem_Items, // VipM_IC_T_Item[]
    bool:MenuItem_UseCounter,
    bool:MenuItem_FakeInactive,
    
    Array:MenuItem_ShowLimits, // T_LimitUnit[]
    Array:MenuItem_ActiveLimits, // T_LimitUnit[]
    Array:MenuItem_Limits, // T_LimitUnit[]
}

bool:MenuItem_Read(const JSON:jMenuItem, MenuItem[S_MenuItem]) {
    new bool:bIsRef, JSON:jRefed = Json_GetRefValue(jMenuItem, .bIsRef = bIsRef);

    if (!json_is_object(jRefed)) {
        Json_LogForFile(jRefed, "WARNING", "Invalid menu item format.");
        return false;
    }

    if (!json_object_get_string(jMenuItem, "Title", MenuItem[MenuItem_Title], charsmax(MenuItem[MenuItem_Title]))) {
        MenuItem[MenuItem_Type] = MenuItemType_Blank;
        Json_FreeExIf(jRefed, bIsRef);
        return true;
    }

    MenuItem[MenuItem_Items] = Json_Object_GetItemsIC(jMenuItem, "Items");

    if (MenuItem[MenuItem_Items] == Invalid_Array) {
        MenuItem[MenuItem_Type] = MenuItemType_Text;
        Json_FreeExIf(jRefed, bIsRef);
        return true;
    }
    
    MenuItem[MenuItem_Type] = MenuItemType_Default;

    MenuItem[MenuItem_ShowLimits] = Json_Object_GetLimits(jMenuItem, "ShowLimits");
    MenuItem[MenuItem_ActiveLimits] = Json_Object_GetLimits(jMenuItem, "ActiveLimits");
    MenuItem[MenuItem_Limits] = Json_Object_GetLimits(jMenuItem, "Limits");
    MenuItem[MenuItem_FakeInactive] = Json_Object_GetBool(jMenuItem, "FakeInactive", false);
    MenuItem[MenuItem_UseCounter] = Json_Object_GetBool(jMenuItem, "UseCounter", true);

    Json_FreeExIf(jRefed, bIsRef);
    return true;
}

Array:MenuItem_ReadList(const JSON:jMenuItems, &Array:aMenuItems = Invalid_Array) {
    ArrayCreateIfNotCreated(aMenuItems, S_MenuItem, 1);

    new bool:bIsRef, JSON:jRefed = Json_GetRefValue(jMenuItems, .bIsRef = bIsRef);
    switch (json_get_type(jRefed)) {
        case JSONObject: {
            new MenuItem[S_MenuItem];
            if (MenuItem_Read(jRefed, MenuItem)) {
                ArrayPushArray(aMenuItems, MenuItem);
            }
        }
        case JSONArray: {
            json_array_foreach_value (jRefed: i => jMenuItem) {
                MenuItem_ReadList(jMenuItem, aMenuItems);
                json_free(jMenuItem);
            }
        }
        default: {
            Json_LogForFile(jRefed, "WARNING", "Invalid menu items format.");
        }
    }

    Json_FreeExIf(jRefed, bIsRef);
    return aMenuItems;
}

Array:Json_Object_GetMenuItemsList(
    const JSON:jObj,
    const sKey[],
    &Array:aMenuItems = Invalid_Array,
    const bool:bDotNot = false
) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return Invalid_Array;
    }

    new JSON:jMenuItems = json_object_get_value(jObj, sKey, bDotNot);
    aMenuItems = MenuItem_ReadList(jMenuItems, aMenuItems);
    json_free(jMenuItems);

    return aMenuItems;
}
