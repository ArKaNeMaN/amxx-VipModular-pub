#include amxmodx
#include VipModular

Menu_MainMenu(const UserId, const MainMenuTitle[], const Array:aMenus){
    new iMenu = menu_create(MainMenuTitle, "@MenuHandler_Command");

    ArrayForeachArray(aMenus => Menu[S_WeaponMenu]){
        static Cmd[128];
        GetClientCmdByLang(Cmd, charsmax(Cmd), CMD_LANG_WEAPON_MENU);
        menu_additem(iMenu, Menu[WeaponMenu_Name], fmt("%s %d", Cmd, __i__));
    }
    menu_display(UserId, iMenu);
}

Menu_WeaponsMenu(const UserId, const MenuId, const Menu[S_WeaponMenu]){
    new iMenu = menu_create(fmt("%s^n\w%L", Menu[WeaponMenu_Title], UserId, "MENU_MAIN_LEFT", gUserLeftItems[UserId]), "@MenuHandler_Command");

    ArrayForeachArray(Menu[WeaponMenu_Items] => MenuItem[S_MenuItem]){
        switch(MenuItem[MenuItem_Type]){
            case MenuItemType_Default: {
                static Cmd[128];
                GetClientCmdByLang(Cmd, charsmax(Cmd), CMD_LANG_WEAPON_MENU);
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

@MenuHandler_Command(const UserId, const MenuId, const ItemId){
    if(ItemId == MENU_EXIT){
        menu_destroy(MenuId);
        return;
    }

    static Data[128];
    menu_item_getinfo(MenuId, ItemId, _, Data, charsmax(Data));
    client_cmd(UserId, Data);
    menu_destroy(MenuId);
}