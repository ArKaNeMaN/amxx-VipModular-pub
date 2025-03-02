#if defined __vipm_weaponmenu_objects_weaponmenu_included
    #endinput
#endif
#define __vipm_weaponmenu_objects_weaponmenu_included

#include <amxmodx>
#include <json>
#include <VipModular>
#include "VipM/JsonUtils"

#include "VipM/WeaponMenu/Objects/MenuItem"

// TODO: Мб на хендлеры переделать?)

enum _:S_WeaponMenu {
    WeaponMenu_Name[64],
    WeaponMenu_Title[128],

    Array:WeaponMenu_Items, // S_MenuItem[]
    WeaponMenu_Count,

    Array:WeaponMenu_Limits, // T_LimitUnit[]

    bool:WeaponMenu_BackOnExit,
    WeaponMenu_PerPage,
    bool:WeaponMenu_ShowPage,
    
    WeaponMenu_FakeMessage[256],
}

bool:WeaponMenu_Read(&JSON:jMenu, Menu[S_WeaponMenu]) {
    new bool:bIsRef, JSON:jRefed = Json_GetRefValue(jMenu, .bIsRef = bIsRef);
    
    Json_Object_GetString(jRefed, "Name", Menu[WeaponMenu_Name], charsmax(Menu[WeaponMenu_Name]));

    if (Json_Object_GetColoredChatMessage(
        jRefed, "FakeMessage",
        Menu[WeaponMenu_FakeMessage], charsmax(Menu[WeaponMenu_FakeMessage])
    )) {
        Json_FreeExIf(jRefed, bIsRef);
        return true;
    }
    
    Menu[WeaponMenu_Items] = Json_Object_GetMenuItemsList(jRefed, "Items");
    if (Menu[WeaponMenu_Items] == Invalid_Array) {
        PCJson_LogForFile(jRefed, "WARNING", "Menu items list are empty.");
        Json_FreeExIf(jRefed, bIsRef);
        return false;
    }

    Json_Object_GetString(jRefed, "Title", Menu[WeaponMenu_Title], charsmax(Menu[WeaponMenu_Title]), Menu[WeaponMenu_Name]);

    Menu[WeaponMenu_Count] = Json_Object_GetInt(jRefed, "Count", -1);
    Menu[WeaponMenu_BackOnExit] = Json_Object_GetBool(jRefed, "BackOnExit", false);
    Menu[WeaponMenu_PerPage] = Json_Object_GetInt(jRefed, "PerPage", -1);
    Menu[WeaponMenu_ShowPage] = Json_Object_GetBool(jRefed, "ShowPage", true);
    Menu[WeaponMenu_Limits] = Json_Object_GetLimits(jRefed, "Limits");

    Json_FreeExIf(jRefed, bIsRef);
    return true;
}

Array:WeaponMenu_ReadList(const JSON:jWeaponMenus, &Array:aWeaponMenus = Invalid_Array) {
    ArrayCreateIfNotCreated(aWeaponMenus, S_WeaponMenu, 1);

    new bool:bIsRef, JSON:jRefed = Json_GetRefValue(jWeaponMenus, .bIsRef = bIsRef);
    switch (json_get_type(jRefed)) {
        case JSONObject: {
            new WeaponMenu[S_WeaponMenu];
            if (WeaponMenu_Read(jRefed, WeaponMenu)) {
                ArrayPushArray(aWeaponMenus, WeaponMenu);
            }
        }
        case JSONArray: {
            json_array_foreach_value (jRefed: i => jWeaponMenu) {
                aWeaponMenus = WeaponMenu_ReadList(jWeaponMenu, aWeaponMenus);
                json_free(jWeaponMenu);
            }
        }
        default: {
            PCJson_LogForFile(jRefed, "WARNING", "Invalid weapon menus format.");
        }
    }

    Json_FreeExIf(jRefed, bIsRef);
    return aWeaponMenus;
}

Array:Json_Object_GetWeaponMenusList(
    const JSON:jObj,
    const sKey[],
    &Array:aWeaponMenus = Invalid_Array,
    const bool:bDotNot = false
) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return Invalid_Array;
    }

    new JSON:jWeaponMenus = json_object_get_value(jObj, sKey, bDotNot);
    aWeaponMenus = WeaponMenu_ReadList(jWeaponMenus, aWeaponMenus);
    json_free(jWeaponMenus);

    return aWeaponMenus;
}
