#if defined __vipm_weaponmenu_objects_weaponmenu_included
    #endinput
#endif
#define __vipm_weaponmenu_objects_weaponmenu_included

#include <amxmodx>
#include <json>
#include <VipModular>
#include <ParamsController>
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
    new JSON:linkedJson;
    PCJson_HandleLinkedValue(jMenu, linkedJson);

    Json_Object_GetString(linkedJson, "Name", Menu[WeaponMenu_Name], charsmax(Menu[WeaponMenu_Name]));

    if (Json_Object_GetColoredChatMessage(
        linkedJson, "FakeMessage",
        Menu[WeaponMenu_FakeMessage], charsmax(Menu[WeaponMenu_FakeMessage])
    )) {
        PCJson_FreeLinked(linkedJson);
        return true;
    }
    
    Menu[WeaponMenu_Items] = Json_Object_GetMenuItemsList(linkedJson, "Items");
    if (Menu[WeaponMenu_Items] == Invalid_Array) {
        PCJson_LogForFile(linkedJson, "WARNING", "Menu items list are empty.");
        PCJson_FreeLinked(linkedJson);
        return false;
    }

    Json_Object_GetString(linkedJson, "Title", Menu[WeaponMenu_Title], charsmax(Menu[WeaponMenu_Title]), Menu[WeaponMenu_Name]);

    Menu[WeaponMenu_Count] = Json_Object_GetInt(linkedJson, "Count", -1);
    Menu[WeaponMenu_BackOnExit] = Json_Object_GetBool(linkedJson, "BackOnExit", false);
    Menu[WeaponMenu_PerPage] = Json_Object_GetInt(linkedJson, "PerPage", -1);
    Menu[WeaponMenu_ShowPage] = Json_Object_GetBool(linkedJson, "ShowPage", true);
    Menu[WeaponMenu_Limits] = Json_Object_GetLimits(linkedJson, "Limits");

    PCJson_FreeLinked(linkedJson);
    return true;
}

Array:WeaponMenu_ReadList(const JSON:jWeaponMenus, &Array:aWeaponMenus = Invalid_Array) {
    ArrayCreateIfNotCreated(aWeaponMenus, S_WeaponMenu, 1);

    new JSON:linkedJson;
    PCJson_HandleLinkedValue(jWeaponMenus, linkedJson);

    switch (json_get_type(linkedJson)) {
        case JSONObject: {
            new WeaponMenu[S_WeaponMenu];
            if (WeaponMenu_Read(linkedJson, WeaponMenu)) {
                ArrayPushArray(aWeaponMenus, WeaponMenu);
            }
        }
        case JSONArray: {
            json_array_foreach_value (linkedJson: i => jWeaponMenu) {
                aWeaponMenus = WeaponMenu_ReadList(jWeaponMenu, aWeaponMenus);
                json_free(jWeaponMenu);
            }
        }
        default: {
            PCJson_LogForFile(linkedJson, "WARNING", "Invalid weapon menus format.");
        }
    }

    PCJson_FreeLinked(linkedJson);
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
