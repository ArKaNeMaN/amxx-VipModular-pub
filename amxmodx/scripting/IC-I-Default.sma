#include <amxmodx>
#include <reapi>
#include <json>
#include <hamsandwich>
#include <VipModular>
#include <ItemsController>
#include "VipM/Utils"

public stock const PluginName[] = "[IC-I] Default";
public stock const PluginVersion[] = IC_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://github.com/ArKaNeMaN/amxx-VipModular-pub";
public stock const PluginDescription[] = "[ItemsController-Item] Default items.";

new Float:g_fSpeedMult[MAX_PLAYERS + 1] = {1.0, ...};
new Float:g_fGivenDmgMult[MAX_PLAYERS + 1] = {1.0, ...};
new Float:g_fTakenDmgMult[MAX_PLAYERS + 1] = {1.0, ...};

new HookChain:g_iHook_ResetMaxSpeed_Post = INVALID_HOOKCHAIN;
new HookChain:g_iHook_TakeDamage_Pre = INVALID_HOOKCHAIN;
new HookChain:g_iHook_Spawn_Pre = INVALID_HOOKCHAIN;

public IC_ItemType_OnInited() {
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    // Затычка для тех мест, где при отсутствии предметов выоезает ошибка/варн
    IC_ItemType_SimpleRegister(
        .name = "None"
    );

    IC_ItemType_SimpleRegister(
        .name = "Random",
        .onRead = "@OnRandomRead",
        .onGive = "@OnRandomGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "InstantReloadAllWeapons",
        .onGive = "@OnInstantReloadAllWeaponsGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "InstantReload",
        .onGive = "@OnInstantReloadGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "RefillBpAmmo",
        .onGive = "@OnRefillBpAmmoGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "Speed",
        .onRead = "@OnSpeedRead",
        .onGive = "@OnSpeedGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "Money",
        .onRead = "@OnMoneyRead",
        .onGive = "@OnMoneyGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "Weapon",
        .onRead = "@OnWeaponRead",
        .onGive = "@OnWeaponGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "ItemsList",
        .onRead = "@OnItemsListRead",
        .onGive = "@OnItemsListGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "Command",
        .onRead = "@OnCommandRead",
        .onGive = "@OnCommandGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "DefuseKit",
        .onGive = "@OnDefuseKitGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "Health",
        .onRead = "@OnHealthRead",
        .onGive = "@OnHealthGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "DamageMult",
        .onRead = "@OnDamageMultRead",
        .onGive = "@OnDamageMultGive"
    );

    IC_ItemType_SimpleRegister(
        .name = "Armor",
        .onRead = "@OnArmorRead",
        .onGive = "@OnArmorGive"
    );

    DisableHookChain(g_iHook_ResetMaxSpeed_Post = RegisterHookChain(RG_CBasePlayer_ResetMaxSpeed, "@OnPlayerResetSpeedPost", true));
    DisableHookChain(g_iHook_Spawn_Pre = RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawnPre", false));
    DisableHookChain(g_iHook_TakeDamage_Pre = RegisterHookChain(RG_CBasePlayer_TakeDamage, "@OnPlayerTakeDamage", false));
}

@OnPlayerSpawnPre(const playerIndex) {
    g_fSpeedMult[playerIndex] = 1.0;
    g_fGivenDmgMult[playerIndex] = 1.0;
    g_fTakenDmgMult[playerIndex] = 1.0;
}

@OnPlayerResetSpeedPost(const playerIndex) {
    if (g_fSpeedMult[playerIndex] != 1.0) {
        MultUserSpeed(playerIndex, g_fSpeedMult[playerIndex]);
    }
}

@OnPlayerTakeDamage(const victimIndex, inflictorIndex, attackerInde, Float:damage, damageType) {
    if (is_user_connected(attackerInde)) {
        damage *= g_fGivenDmgMult[attackerInde];
    }

    if (is_user_connected(victimIndex)) {
        damage *= g_fTakenDmgMult[victimIndex];
    }

    SetHookChainArg(4, ATYPE_FLOAT, damage);
    return HC_CONTINUE;
}

@OnDamageMultRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (json_object_has_value(instanceJson, "Given", JSONNumber)) {
        TrieSetCell(p, "Given", json_object_get_real(instanceJson, "Given"));
    }

    if (json_object_has_value(instanceJson, "Taken", JSONNumber)) {
        TrieSetCell(p, "Taken", json_object_get_real(instanceJson, "Taken"));
    }

    CallOnceR(IC_RET_READ_SUCCESS); // Чтобы лишний раз не дёргать натив активации хука
    
    EnableHookChain(g_iHook_Spawn_Pre);
    EnableHookChain(g_iHook_TakeDamage_Pre);

    return IC_RET_READ_SUCCESS;
}

@OnDamageMultGive(const playerIndex, const Trie:p) {
    g_fGivenDmgMult[playerIndex] = VipM_Params_GetFloat(p, "Given", 1.0);
    g_fTakenDmgMult[playerIndex] = VipM_Params_GetFloat(p, "Taken", 1.0);
}

@OnHealthRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Health", JSONNumber)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Health` required for `Health` item.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Health", json_object_get_real(instanceJson, "Health"));

    if (json_object_has_value(instanceJson, "MaxHealth", JSONNumber)) {
        TrieSetCell(p, "MaxHealth", json_object_get_real(instanceJson, "MaxHealth"));
    }

    if (json_object_has_value(instanceJson, "SetHealth", JSONBoolean)) {
        TrieSetCell(p, "SetHealth", json_object_get_bool(instanceJson, "SetHealth"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnHealthGive(const playerIndex, const Trie:p) {
    if (VipM_Params_GetBool(p, "SetHealth", false)) {
        set_entvar(playerIndex, var_health, VipM_Params_GetFloat(p, "Health"));
    } else {
        new Float:fHealth = Float:get_entvar(playerIndex, var_health);
        new Float:fMaxHealth = VipM_Params_GetFloat(p, "MaxHealth", 100.0);
        new Float:fAddHealth = floatclamp(VipM_Params_GetFloat(p, "Health"), 0.0, floatmax(0.0, fMaxHealth - fHealth));
        new Float:fMaxHealthCurrent = Float:get_entvar(playerIndex, var_max_health);
        new bool:bNeedOverrideMaxHealth = (fHealth < fMaxHealth && fMaxHealthCurrent < fMaxHealth);

        if (bNeedOverrideMaxHealth) {
            set_entvar(playerIndex, var_max_health, fMaxHealth);
        }

        ExecuteHamB(Ham_TakeHealth, playerIndex, fAddHealth, DMG_GENERIC);

        if (bNeedOverrideMaxHealth) {
            set_entvar(playerIndex, var_max_health, fMaxHealthCurrent);
        }
    }
}

@OnArmorRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Armor", JSONNumber)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Armor` required for `Armor` item.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Armor", json_object_get_number(instanceJson, "Armor"));

    if (json_object_has_value(instanceJson, "MaxArmor", JSONNumber)) {
        TrieSetCell(p, "MaxArmor", json_object_get_number(instanceJson, "MaxArmor"));
    }

    if (json_object_has_value(instanceJson, "SetArmor", JSONBoolean)) {
        TrieSetCell(p, "SetArmor", json_object_get_bool(instanceJson, "SetArmor"));
    }

    if (json_object_has_value(instanceJson, "Helmet", JSONBoolean)) {
        TrieSetCell(p, "Helmet", json_object_get_bool(instanceJson, "Helmet"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnArmorGive(const playerIndex, const Trie:p) {
    if (VipM_Params_GetBool(p, "SetArmor", false)) {
        rg_set_user_armor(playerIndex, VipM_Params_GetInt(p, "Armor"), VipM_Params_GetBool(p, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR);
    } else {
        new iSetArmor = min(rg_get_user_armor(playerIndex) + VipM_Params_GetInt(p, "Armor"), VipM_Params_GetInt(p, "MaxArmor", 100));

        rg_set_user_armor(playerIndex, iSetArmor, VipM_Params_GetBool(p, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR);
    }
}

@OnRandomRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    // TODO: Сделать как-то разные шансы
    if (!json_object_has_value(instanceJson, "Items")) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Items` required for item `Random`.");
        return IC_RET_READ_FAIL;
    }
    
    new Array:aItems = Json_Object_IC_GetItems(instanceJson, "Items");
    if (ArraySizeSafe(aItems) <= 1) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Items` must have >1 items.");
        ArrayDestroy(aItems);
        return IC_RET_READ_FAIL;
    }

    TrieSetCell(p, "Items", aItems);

    return IC_RET_READ_SUCCESS;
}

@OnRandomGive(const playerIndex, const Trie:p) {
    new Array:aItems = VipM_Params_GetArr(p, "Items");
    new iRandomIndex = random_num(0, ArraySizeSafe(aItems) - 1);
    new VipM_IC_T_Item:iRandomItem = ArrayGetCell(aItems, iRandomIndex);

    return IC_Item_Give(playerIndex, iRandomItem) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}

@OnInstantReloadGive(const playerIndex, const Trie:p) {
    InstantReloadActiveWeapon(playerIndex);
}

@OnInstantReloadAllWeaponsGive(const playerIndex, const Trie:p) {
    InstantReloadAllWeapons(playerIndex);
}

@OnRefillBpAmmoGive(const playerIndex, const Trie:p) {
    new iMaxAmmos[32] = {-1, ...};

    // Получение дефолтных значений
    for (new WeaponIdType:iWpnId = WEAPON_P228; iWpnId < WEAPON_P90; iWpnId++) {
        switch (iWpnId) {
            case WEAPON_KNIFE, WEAPON_HEGRENADE, WEAPON_SMOKEGRENADE, WEAPON_FLASHBANG, WEAPON_SHIELDGUN:
                iMaxAmmos[rg_get_weapon_info(iWpnId, WI_AMMO_TYPE)] = -1;
            default:
                iMaxAmmos[rg_get_weapon_info(iWpnId, WI_AMMO_TYPE)] = rg_get_weapon_info(iWpnId, WI_MAX_ROUNDS);
        }
    }

    // Получение актуальных значений исходя из пушек в инвентаре 
    for (new InventorySlotType:iSlot = PRIMARY_WEAPON_SLOT; iSlot <= PISTOL_SLOT; iSlot++) {
        new ItemId = get_member(playerIndex, m_rgpPlayerItems, iSlot);
        while (ItemId > 0 && !is_nullent(ItemId)) {
            new iAmmoType = get_member(ItemId, m_Weapon_iPrimaryAmmoType);
            if (iAmmoType >= 0) {
                iMaxAmmos[iAmmoType] = max(iMaxAmmos[iAmmoType], rg_get_iteminfo(ItemId, ItemInfo_iMaxAmmo1));
            }
            ItemId = get_member(ItemId, m_pNext);
        }
    }

    for (new iAmmoType = 0; iAmmoType < 32; iAmmoType++) {
        if (iMaxAmmos[iAmmoType] >= 0) {
            set_member(playerIndex, m_rgAmmo, iMaxAmmos[iAmmoType], iAmmoType);
        }
    }
}

@OnSpeedRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Multiplier", JSONNumber)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Multiplier` required for item `Speed`.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Multiplier", json_object_get_real(instanceJson, "Multiplier"));

    CallOnceR(IC_RET_READ_SUCCESS); // Чтобы лишний раз не дёргать натив активации хука

    EnableHookChain(g_iHook_Spawn_Pre);
    EnableHookChain(g_iHook_ResetMaxSpeed_Post);

    return IC_RET_READ_SUCCESS;
}

@OnSpeedGive(const playerIndex, const Trie:p) {
    g_fSpeedMult[playerIndex] = VipM_Params_GetFloat(p, "Multiplier", 1.0);
    MultUserSpeed(playerIndex, g_fSpeedMult[playerIndex]);
}

@OnMoneyRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Amount", JSONNumber)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Amount` required for item `Money`.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Amount", json_object_get_number(instanceJson, "Amount"));

    if (json_object_has_value(instanceJson, "GiveType", JSONString)) {
        new sBuffer[32];
        json_object_get_string(instanceJson, "GiveType", sBuffer, charsmax(sBuffer));
        new accountSet = StrToAccountSet(sBuffer);
        if (accountSet < 0) {
            Json_LogForFile(instanceJson, "ERROR", "Invalid `GiveType` value (%s). Expected `Set` or `Add`.", sBuffer);
        } else {
            TrieSetCell(p, "GiveType", accountSet);
        }
    }

    if (json_object_has_value(instanceJson, "TrackChange", JSONBoolean)) {
        TrieSetCell(p, "TrackChange", json_object_get_bool(instanceJson, "TrackChange"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnMoneyGive(const playerIndex, const Trie:p) {
    rg_add_account(
        playerIndex,
        VipM_Params_GetInt(p, "Amount"),
        VipM_Params_GetCell(p, "GiveType", AS_ADD),
        VipM_Params_GetBool(p, "TrackChanges", true)
    );
}

@OnItemsListRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Items")) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Items` required for item `ItemsList`.");
        return IC_RET_READ_FAIL;
    }

    TrieSetCell(p, "Items", Json_Object_IC_GetItems(instanceJson, "Items"));

    return IC_RET_READ_SUCCESS;
}

@OnItemsListGive(const playerIndex, const Trie:p) {
    new Array:aItems = VipM_Params_GetCell(p, "Items", Invalid_Array);

    return IC_Item_GiveArray(playerIndex, aItems) ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}

@OnCommandRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Command", JSONString)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Command` required for item `Command`.");
        return IC_RET_READ_FAIL;
    }
    
    new Command[128];
    json_object_get_string(instanceJson, "Command", Command, charsmax(Command));
    TrieSetString(p, "Command", Command);

    if (json_object_has_value(instanceJson, "ByServer", JSONBoolean)) {
        TrieSetCell(p, "ByServer", json_object_get_bool(instanceJson, "ByServer"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnCommandGive(const playerIndex, const Trie:p) {
    static Command[128];
    VipM_Params_GetStr(p, "Command", Command, charsmax(Command));
    new bool:ByServer = VipM_Params_GetBool(p, "ByServer", false);

    replace_all(Command, charsmax(Command), "{playerIndex}", IntToStr(playerIndex));

    if (ByServer) {
        server_cmd(Command);
    } else {
        client_cmd(playerIndex, Command);
    }
}

@OnDefuseKitGive(const playerIndex, const Trie:p) {
    if (get_member(playerIndex, m_iTeam) == TEAM_CT) {
        rg_give_defusekit(playerIndex);
    }
}

@OnWeaponRead(const JSON:instanceJson, const Trie:p) {
    new Name[32];
    json_object_get_string(instanceJson, "Name", Name, charsmax(Name));

    // if(get_weaponid(Name) == 0){
    //     log_amx("[WARNING] Weapon `%s` not found.", Name);
    //     return IC_RET_READ_FAIL;
    // }

    TrieSetCell(p, "GiveType", _:Json_Object_GetGiveType(instanceJson, "GiveType"));

    if (json_object_has_value(instanceJson, "BpAmmo", JSONNumber)) {
        TrieSetCell(p, "BpAmmo", json_object_get_number(instanceJson, "BpAmmo"));
    }

    TrieSetString(p, "Name", Name);

    return IC_RET_READ_SUCCESS;
}

@OnWeaponGive(const playerIndex, const Trie:p) {
    static WeaponName[32];
    VipM_Params_GetStr(p, "Name", WeaponName, charsmax(WeaponName));
    if (!WeaponName[0]) {
        return IC_RET_GIVE_FAIL;
    }
    
    if (!get_weaponid(WeaponName)) {
        log_amx("[WARNING] Default weapon `%s` not found.", WeaponName);
        return IC_RET_GIVE_FAIL;
    }
    
    new GiveType:iGiveType = GiveType:VipM_Params_GetInt(p, "GiveType", _:GT_DROP_AND_REPLACE);
    new iBpAmmo = VipM_Params_GetInt(p, "BpAmmo", -1);

    new ItemId = rg_give_item(playerIndex, WeaponName, iGiveType);
    if (ItemId < 0) {
        return IC_RET_GIVE_FAIL;
    }

    new WeaponIdType:iWpnId = rg_get_weapon_info(WeaponName, WI_ID);
    new iWpnSlot = rg_get_iteminfo(ItemId, ItemInfo_iSlot);

    if (
        iBpAmmo < 0
        && !(
            iWpnSlot == 0
            || iWpnSlot == 1
        )
    ) {
        return IC_RET_GIVE_SUCCESS;
    }

    new def_BpAmmo = (rg_get_weapon_info(iWpnId, WI_MAX_ROUNDS));
    if (def_BpAmmo >= 0) {
        rg_set_user_bpammo(playerIndex, iWpnId, iBpAmmo < 0 ? def_BpAmmo : iBpAmmo);
    }

    return IC_RET_GIVE_SUCCESS;
}

GiveType:Json_Object_GetGiveType(const JSON:Obj, const Key[], const bool:DotNot = false) {
    new Str[32];
    json_object_get_string(Obj, Key, Str, charsmax(Str), DotNot);
    return StrToGiveType(Str);
}

GiveType:StrToGiveType(const Str[]) {
    if (equali(Str, "GT_APPEND") || equali(Str, "Append") || equali(Str, "Add")) {
        return GT_APPEND;
    } else if (equali(Str, "GT_REPLACE") || equali(Str, "Replace")) {
        return GT_REPLACE;
    } else if (equali(Str, "GT_DROP_AND_REPLACE") || equali(Str, "Drop") || equali(Str, "DropAndReplace")) {
        return GT_DROP_AND_REPLACE;
    } else {
        return GT_DROP_AND_REPLACE;
    }
}

StrToAccountSet(const Str[]) {
    if (equali(Str, "Set")) {
        return _:AS_SET;
    } else if (equali(Str, "Add")) {
        return _:AS_ADD;
    } else {
        return -1;
    }
}

MultUserSpeed(const playerIndex, const Float:fMultiplier) {
    set_entvar(playerIndex, var_maxspeed, Float:get_entvar(playerIndex, var_maxspeed) * fMultiplier);
}
