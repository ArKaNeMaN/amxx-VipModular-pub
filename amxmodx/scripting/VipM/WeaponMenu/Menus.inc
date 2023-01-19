#include amxmodx
#include VipModular
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"
#include "VipM/DebugMode"

Menu_GetUserMenusCount(const UserId) {
    return ArraySizeSafe(VipM_Params_GetArr(VipM_Modules_GetParams(MODULE_NAME, UserId), "Menus"));
}

Menu_MainMenu(const UserId, const MainMenuTitle[], const Array:aMenus){
    new iMenu = menu_create(MainMenuTitle, "@MenuHandler_Command");

    ArrayForeachArray(aMenus => Menu[S_WeaponMenu]) {
        static Cmd[128];
        CommandAliases_GetMainCmd(CMD_WEAPON_MENU, Cmd, charsmax(Cmd));
        menu_additem(iMenu, Menu[WeaponMenu_Name], fmt("%s %d", Cmd, __i__));
    }
    menu_display(UserId, iMenu);
}

Menu_WeaponsMenu(const UserId, const MenuId, const Menu[S_WeaponMenu]) {
    new iItemsLeft = GetUserLeftItems(UserId, MenuId, Menu);
    new iMenusCount = Menu_GetUserMenusCount(UserId);

    Dbg_Log("Menu_WeaponsMenu(%n, %d, ...):", UserId, MenuId);
    Dbg_Log("    Menu[WeaponMenu_Count] = %d", Menu[WeaponMenu_Count]);
    Dbg_Log("    iItemsLeft = %d", iItemsLeft);

    new iMenu = menu_create(
        fmt("%s^n\w%L^n", Menu[WeaponMenu_Title], UserId, "VIPM_M_WM_MENU_MAIN_ITEMS_LEFT", iItemsLeft),
        Menu[WeaponMenu_BackOnExit] && iMenusCount > 1
            ? "@MenuHandler_Weapons"
            : "@MenuHandler_Command"
    );

    if (Menu[WeaponMenu_BackOnExit] && iMenusCount > 1) {
        menu_setprop(iMenu, MPROP_EXITNAME, uLang(UserId, "VIPM_M_WM_MENU_WEAPONS_BACK_TO_MAIN_TEXT"));
    }

    ArrayForeachArray(Menu[WeaponMenu_Items] => MenuItem[S_MenuItem]){
        switch (MenuItem[MenuItem_Type]) {
            case MenuItemType_Default: {
                static Cmd[128];
                CommandAliases_GetMainCmd(CMD_WEAPON_MENU, Cmd, charsmax(Cmd));
                menu_additem(iMenu, MenuItem[MenuItem_Title], fmt("%s %d %d", Cmd, MenuId, __i__));
            }
            
            case MenuItemType_Text: {
                menu_addtext2(iMenu, MenuItem[MenuItem_Title]);
            }

            case MenuItemType_Blank: {
                menu_addblank2(iMenu);
            }
        }
    }
    menu_display(UserId, iMenu);
}

@MenuHandler_Weapons(const UserId, const MenuId, const ItemId) {
    @MenuHandler_Command(UserId, MenuId, ItemId);
    
    if (ItemId == MENU_EXIT) {
        CommandAliases_ClientCmd(UserId, CMD_WEAPON_MENU);
    }
}

@MenuHandler_Command(const UserId, const MenuId, const ItemId){
    if (ItemId == MENU_EXIT) {
        menu_destroy(MenuId);
        return;
    }

    static Data[128];
    menu_item_getinfo(MenuId, ItemId, _, Data, charsmax(Data));
    client_cmd(UserId, Data);
    menu_destroy(MenuId);
}