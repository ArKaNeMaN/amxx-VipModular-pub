#include amxmodx
#include VipModular
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"

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
    new iItemsLeft = gUserLeftItems[UserId];
    if (Menu[WeaponMenu_Count]) {
        iItemsLeft = min(iItemsLeft, Menu[WeaponMenu_Count] - KeyValueCounter_Get(g_tUserMenuItemsCounter[UserId], IntToStr(MenuId)));
    }
    // Или лучше будет показывать оба лимита отдельно?

    new iMenusCount = Menu_GetUserMenusCount(UserId);

    new iMenu = menu_create(
        fmt("%s^n\w%L", Menu[WeaponMenu_Title], UserId, "VIPM_M_WM_MENU_MAIN_ITEMS_LEFT", iItemsLeft),
        Menu[WeaponMenu_BackOnExit] && iMenusCount > 1
            ? "@MenuHandler_Command"
            : "@MenuHandler_Weapons"
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