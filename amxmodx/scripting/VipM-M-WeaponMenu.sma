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

new const CMD_LANG_WEAPON_MENU[] = "CMD_OPEN_WEAPON_MENU_";
new const CMD_LANG_AUTOOPEN[] = "CMD_SWITCH_AUTOOPEN_";

#include "VipM/ArrayTrieUtils"
#include "VipM/Utils"

new gUserLeftItems[MAX_PLAYERS + 1] = {0, ...};
new bool:gUserAutoOpen[MAX_PLAYERS + 1] = {true, ...};

#include "VipM/WeaponMenu/Structs"
#include "VipM/WeaponMenu/Configs"
#include "VipM/WeaponMenu/Menus"

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);
    register_dictionary("VipM-WeaponMenu.ini");

    VipM_Modules_Register(MODULE_NAME, true);
    VipM_Modules_AddParams(MODULE_NAME,
        "MainMenuTitle", ptString, false,
        // "Menus", ptCustom, true,
        "MinRound", ptInteger, false,
        "Count", ptInteger, false,
        "CheckPrimaryWeapon", ptBoolean, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@OnModuleActivate");
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnRead, "@OnReadConfig");
}

@OnReadConfig(const JSON:jCfg, Trie:Params){
    if(!json_object_has_value(jCfg, "Menus", JSONArray)){
        log_amx("[WARNING] Param `Menus` required for module `%s`.", MODULE_NAME);
        return VIPM_STOP;
    }

    new JSON:jMenus = json_object_get_value(jCfg, "Menus");
    new Array:aMenus = Cfg_ReadMenus(jMenus);
    TrieSetCell(Params, "Menus", aMenus);

    if(!TrieKeyExists(Params, "MainMenuTitle"))
        TrieSetString(Params, "MainMenuTitle", Lang("MENU_MAIN_TITLE"));

    return VIPM_CONTINUE;
}

@OnModuleActivate(){
    RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawn", true);

    RegisterClientCommandByLang(CMD_LANG_WEAPON_MENU, "@Cmd_OpenMenu");
    RegisterClientCommandByLang(CMD_LANG_AUTOOPEN, "@Cmd_SwitchAutoOpen");
}

@OnPlayerSpawn(const UserId){
    if(!IsUserValidA(UserId))
        return;

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    gUserLeftItems[UserId] = VipM_Params_GetInt(Params, "Count", 0);

    if(!gUserAutoOpen[UserId])
        return;

    if(VipM_Params_GetArr(Params, "Menus") == Invalid_Array)
        return;

    if(GetRound() < VipM_Params_GetInt(Params, "MinRound", 0))
        return;

    if(
        VipM_Params_GetBool(Params, "CheckPrimaryWeapon", false)
        && get_member(UserId, m_bHasPrimary)
    ) return;

    ClientCmdByLang(UserId, CMD_LANG_WEAPON_MENU, "");
}

@Cmd_SwitchAutoOpen(const UserId){
    gUserAutoOpen[UserId] = !gUserAutoOpen[UserId];
    ChatPrintL(UserId, gUserAutoOpen[UserId] ? "MSG_AUTOOPEN_TURNED_ON" : "MSG_AUTOOPEN_TURNED_OFF");
}

@Cmd_OpenMenu(const UserId){
    if(!IsUserValid(UserId))
        return;

    if(!is_user_alive(UserId)){
        ChatPrintL(UserId, "MSG_YOU_DEAD");
        return;
    }

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    new Array:aMenus = VipM_Params_GetArr(Params, "Menus");

    if(ArraySizeSafe(aMenus) < 1){
        ChatPrintL(UserId, "MSG_NO_ACCESS");
        return;
    }

    new MinRound = VipM_Params_GetInt(Params, "MinRound", 0);
    // log_amx("[DEBUG] @Cmd_OpenMenu: MinRound = %d", MinRound);
    // log_amx("[DEBUG] @Cmd_OpenMenu: GetRound() = %d", GetRound());
    if(GetRound() < MinRound){
        ChatPrintL(UserId, "MSG_MAIN_MIN_ROUND", MinRound);
        return;
    }

    CMD_INIT_PARAMS();

    if(CMD_ARG_NUM() < 1){
        if(ArraySizeSafe(aMenus) == 1)
            ClientCmdByLang(UserId, CMD_LANG_WEAPON_MENU, "0");
        else{
            static MainMenuTitle[128];
            VipM_Params_GetStr(Params, "MainMenuTitle", MainMenuTitle, charsmax(MainMenuTitle));
            Menu_MainMenu(UserId, MainMenuTitle, aMenus);
        }
        return;
    }

    new MenuId = read_argv_int(CMD_ARG(1));
    if(
        ArraySizeSafe(aMenus) <= MenuId
        || MenuId < 0
    ) return;

    static Menu[S_WeaponMenu];
    ArrayGetArray(aMenus, MenuId, Menu);

    if(GetRound() < Menu[WeaponMenu_MinRound]){
        ChatPrintL(UserId, "MSG_MENU_MIN_ROUND", Menu[WeaponMenu_MinRound]);
        return;
    }
    
    if(CMD_ARG_NUM() < 2){
        Menu_WeaponsMenu(UserId, MenuId, Menu);
        return;
    }

    new ItemId = read_argv_int(CMD_ARG(2));
    if(
        ArraySizeSafe(Menu[WeaponMenu_Items]) <= ItemId
        || ItemId < 0
    ) return;

    static MenuItem[S_MenuItem];
    ArrayGetArray(Menu[WeaponMenu_Items], ItemId, MenuItem);

    if(gUserLeftItems[UserId] < 1){
        ChatPrintL(UserId, "MSG_NO_LEFT_ITEMS");
        return;
    }

    if(GetRound() < MenuItem[MenuItem_MinRound]){
        ChatPrintL(UserId, "MSG_MENUITEM_MIN_ROUND", MenuItem[MenuItem_MinRound]);
        return;
    }
    
    if(VipM_IC_GiveItems(UserId, MenuItem[MenuItem_Items]))
        gUserLeftItems[UserId]--;
}
