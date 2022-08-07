#include amxmodx
#include json
#include VipModular

Array:Cfg_ReadMenus(&JSON:jMenus){
    new Array:aMenus = Invalid_Array;
    new Menu[S_WeaponMenu];

    // TODO: Заменить на Json_IsRef
    jMenus = JSON_GET_FILE_OR_OBJECT(jMenus);

    switch (json_get_type(jMenus)) {
        case JSONArray: {
            new cMenus = json_array_get_count(jMenus);
            aMenus = ArrayCreate(S_WeaponMenu, cMenus);

            for (new i = 0; i < cMenus; i++) {
                new JSON:jMenu = json_array_get_value(jMenus, i);
                if (Cfg_ReadMenu(jMenu, Menu)) {
                    ArrayPushArray(aMenus, Menu);
                }
            }
            json_free(jMenus);
        }

        case JSONObject, JSONString: {
            if (Cfg_ReadMenu(jMenus, Menu)) {
                aMenus = ArrayCreate(S_WeaponMenu, 1);
                ArrayPushArray(aMenus, Menu);
            }
        }
        default:
            json_free(jMenus);
    }
    return aMenus;
}

bool:Cfg_ReadMenu(&JSON:jMenu, Menu[S_WeaponMenu]){
    static Trie:_FileCache;
    if (_FileCache == Invalid_Trie) {
        _FileCache = TrieCreate();
    }

    jMenu = JSON_GET_FILE_OR_OBJECT(jMenu);
    if (jMenu == Invalid_JSON) {
        return false;
    }

    json_object_get_string(jMenu, "Name", Menu[WeaponMenu_Name], charsmax(Menu[WeaponMenu_Name]));
    json_object_get_string(jMenu, "Title", Menu[WeaponMenu_Title], charsmax(Menu[WeaponMenu_Title]));
    Menu[WeaponMenu_Count] = json_object_get_number(jMenu, "Count");
    Menu[WeaponMenu_Limits] = VipM_Limits_ReadListFromJson(json_object_get_value(jMenu, "Limits"));

    new JSON:jMenuItems = JsonObjectGetItemsValue(jMenu);
    Menu[WeaponMenu_Items] = Cfg_ReadMenuItems(jMenuItems);

    json_free(jMenu);

    if (Menu[WeaponMenu_Items] == Invalid_Array) {
        log_amx("[WARNING] Menu items are not specified.");
        return false;
    }

    return true;
}

Array:Cfg_ReadMenuItems(&JSON:jMenuItems){
    new Array:aMenuItems = Invalid_Array;
    new MenuItem[S_MenuItem];

    jMenuItems = JSON_GET_FILE_OR_OBJECT(jMenuItems);

    switch (json_get_type(jMenuItems)) {
        case JSONArray: {
            new cMenuItems = json_array_get_count(jMenuItems);
            aMenuItems = ArrayCreate(S_MenuItem, cMenuItems);

            for (new i = 0; i < cMenuItems; i++) {
                new JSON:jMenuItem = json_array_get_value(jMenuItems, i);
                if(Cfg_ReadMenuItem(jMenuItem, MenuItem))
                    ArrayPushArray(aMenuItems, MenuItem);
            }
            json_free(jMenuItems);
        }

        case JSONObject: {
            if (Cfg_ReadMenuItem(jMenuItems, MenuItem)) {
                aMenuItems = ArrayCreate(S_MenuItem, 1);
                ArrayPushArray(aMenuItems, MenuItem);
            }
        }
        default:
            json_free(jMenuItems);
    }
    return aMenuItems;
}

bool:Cfg_ReadMenuItem(&JSON:jMenuItem, MenuItem[S_MenuItem]){
    if (!json_is_object(jMenuItem)) {
        log_amx("[WARNING] Invalid weapon menu item format.");
        return false;
    }

    if (!json_object_get_string(jMenuItem, "Title", MenuItem[MenuItem_Title], charsmax(MenuItem[MenuItem_Title]))) {
        MenuItem[MenuItem_Type] = MenuItemType_Blank;
        return true;
    }

    new JSON:jItems = JsonObjectGetItemsValue(jMenuItem);
    if (jItems == Invalid_JSON) {
        MenuItem[MenuItem_Type] = MenuItemType_Text;
        return true;
    }
    
    MenuItem[MenuItem_Type] = MenuItemType_Default;
    MenuItem[MenuItem_Limits] = VipM_Limits_ReadListFromJson(json_object_get_value(jMenuItem, "Limits"));
    json_free(jMenuItem);

    MenuItem[MenuItem_Items] = VipM_IC_JsonGetItems(jItems);
    if (MenuItem[MenuItem_Items] == Invalid_Array) {
        log_amx("[WARNING] Invalid weapon menu item format. Empty items list.");
        return false;
    }

    return true;
}

JSON:JsonObjectGetItemsValue(const JSON:jObj){
    if (json_object_has_value(jObj, "Items")) {
        return json_object_get_value(jObj, "Items");    
    } else if (json_object_has_value(jObj, "Item")) {
        return json_object_get_value(jObj, "Item");
    } else {
        return Invalid_JSON;
    }
}