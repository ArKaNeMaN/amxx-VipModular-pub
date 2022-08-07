#include <amxmodx>
#include <reapi>
#include <VipModular>

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][M] Weapon Menu";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Vip modular`s module - Weapon Menu";

new const MODULE_NAME[] = "WeaponMenu";

new const CMD_WEAPON_MENU[] = "vipmenu";
new const CMD_WEAPON_MENU_SILENT[] = "vipmenu_silent";
new const CMD_SWITCH_AUTOOPEN[] = "vipmenu_autoopen";

#include "VipM/ArrayTrieUtils"
#include "VipM/Utils"
#include "VipM/CommandAliases"

new gUserLeftItems[MAX_PLAYERS + 1] = {0, ...};
new Trie:g_tUserMenuItemsCounter[MAX_PLAYERS + 1] = {Invalid_Trie, ...};

new bool:gUserAutoOpen[MAX_PLAYERS + 1] = {true, ...};
new bool:gUserExecutedAutoOpen[MAX_PLAYERS + 1] = {false, ...};

#include "VipM/WeaponMenu/KeyValueCounter"
#include "VipM/WeaponMenu/Structs"
#include "VipM/WeaponMenu/Configs"
#include "VipM/WeaponMenu/Menus"

public VipM_OnInitModules() {
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);
    register_dictionary("VipM-WeaponMenu.ini");

    VipM_Modules_Register(MODULE_NAME, true);
    VipM_Modules_AddParams(MODULE_NAME,
        "MainMenuTitle", ptString, false,
        "Menus", ptCustom, true,
        "Count", ptInteger, false
    );
    VipM_Modules_AddParams(MODULE_NAME,
        "Limits", ptLimits, false,
        "AutoopenLimits", ptLimits, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnRead, "@OnReadConfig");
}

@OnReadConfig(const JSON:jCfg, Trie:Params) {
    if (!json_object_has_value(jCfg, "Menus", JSONArray)) {
        log_amx("[WARNING] Param `Menus` required for module `%s`.", MODULE_NAME);
        return VIPM_STOP;
    }

    new JSON:jMenus = json_object_get_value(jCfg, "Menus");
    new Array:aMenus = Cfg_ReadMenus(jMenus);
    TrieSetCell(Params, "Menus", aMenus);

    if (!TrieKeyExists(Params, "MainMenuTitle")) {
        TrieSetString(Params, "MainMenuTitle", Lang("MENU_MAIN_TITLE"));
    }

    return VIPM_CONTINUE;
}

@OnModuleActivate() {
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawn", true);
    
    CommandAliases_Open(GET_FILE_JSON_PATH("Cmds/WeaponMenu"), true);
    CommandAliases_RegisterClient(CMD_WEAPON_MENU, "@Cmd_Menu");
    CommandAliases_RegisterClient(CMD_WEAPON_MENU_SILENT, "@Cmd_MenuSilent");
    CommandAliases_RegisterClient(CMD_SWITCH_AUTOOPEN, "@Cmd_SwitchAutoOpen");
    CommandAliases_Close();
}

@OnPlayerSpawn(const UserId) {
    if (!IsUserValidA(UserId)) {
        return;
    }

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    gUserLeftItems[UserId] = VipM_Params_GetInt(Params, "Count", 0);
    g_tUserMenuItemsCounter[UserId] = KeyValueCounter_Reset(g_tUserMenuItemsCounter[UserId]);

    if (!gUserAutoOpen[UserId]) {
        return;
    }

    if (VipM_Params_GetArr(Params, "Menus") == Invalid_Array) {
        return;
    }

    if (!VipM_Params_ExecuteLimitsList(Params, "AutoopenLimits", UserId, Limit_Exec_AND)) {
        return;
    }

    gUserExecutedAutoOpen[UserId] = true;
    CommandAliases_ClientCmd(UserId, CMD_WEAPON_MENU);
}

@Cmd_SwitchAutoOpen(const UserId) {
    gUserAutoOpen[UserId] = !gUserAutoOpen[UserId];
    ChatPrintL(UserId, gUserAutoOpen[UserId] ? "MSG_AUTOOPEN_TURNED_ON" : "MSG_AUTOOPEN_TURNED_OFF");
}

@Cmd_Menu(const UserId) {
    _Cmd_Menu(UserId);
}

@Cmd_MenuSilent(const UserId) {
    _Cmd_Menu(UserId, true);
}

_Cmd_Menu(const UserId, const bool:bSilent = false) {
    if (!IsUserValid(UserId)) {
        return;
    }

    if (!is_user_alive(UserId)) {
        if (!bSilent) {
            ChatPrintL(UserId, "MSG_YOU_DEAD");
        }

        return;
    }

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    new Array:aMenus = VipM_Params_GetArr(Params, "Menus");

    if (ArraySizeSafe(aMenus) < 1) {
        if (!bSilent) {
            ChatPrintL(UserId, "MSG_NO_ACCESS");
        }

        return;
    }
    
    if (!VipM_Params_ExecuteLimitsList(Params, "Limits", UserId, Limit_Exec_AND)) {
        if (!bSilent) {
            ChatPrintL(UserId, "MSG_MAIN_NOT_PASSED_LIMIT");
        }

        return;
    }

    CMD_INIT_PARAMS();

    if (CMD_ARG_NUM() < 1) {
        if (ArraySizeSafe(aMenus) == 1) {
            CommandAliases_ClientCmd(UserId, CMD_WEAPON_MENU, "0");
        } else {
            static MainMenuTitle[128];
            VipM_Params_GetStr(Params, "MainMenuTitle", MainMenuTitle, charsmax(MainMenuTitle));
            Menu_MainMenu(UserId, MainMenuTitle, aMenus);
        }
        return;
    }

    new MenuId = read_argv_int(CMD_ARG(1));
    if (
        ArraySizeSafe(aMenus) <= MenuId
        || MenuId < 0
    ) {
        return;
    }

    static Menu[S_WeaponMenu];
    ArrayGetArray(aMenus, MenuId, Menu);

    if (!VipM_Limits_ExecuteList(Menu[WeaponMenu_Limits], UserId)) {
        if (!bSilent) {
            ChatPrintL(UserId, "MSG_MENU_NOT_PASSED_LIMIT");
        }

        return;
    }
    
    if (CMD_ARG_NUM() < 2) {
        Menu_WeaponsMenu(UserId, MenuId, Menu);
        return;
    }

    new ItemId = read_argv_int(CMD_ARG(2));
    if (
        ArraySizeSafe(Menu[WeaponMenu_Items]) <= ItemId
        || ItemId < 0
    ) {
        return;
    }

    static MenuItem[S_MenuItem];
    ArrayGetArray(Menu[WeaponMenu_Items], ItemId, MenuItem);

    if (
        // Общий лимит
        gUserLeftItems[UserId] < 1
        // Лимит на конкретном меню
        || KeyValueCounter_Get(g_tUserMenuItemsCounter[UserId], IntToStr(MenuId)) >= Menu[WeaponMenu_Count]
    ) {
        if (!bSilent) {
            ChatPrintL(UserId, "MSG_NO_LEFT_ITEMS");
        }
        
        return;
    }

    if (!VipM_Limits_ExecuteList(MenuItem[MenuItem_Limits], UserId)) {
        if (!bSilent) {
            ChatPrintL(UserId, "MSG_MENUITEM_NOT_PASSED_LIMIT");
        }
        
        return;
    }
    
    if (VipM_IC_GiveItems(UserId, MenuItem[MenuItem_Items])) {
        gUserLeftItems[UserId]--;

        if (Menu[WeaponMenu_Count]) {
            KeyValueCounter_Inc(g_tUserMenuItemsCounter[UserId], IntToStr(MenuId));
        }
    }
}
