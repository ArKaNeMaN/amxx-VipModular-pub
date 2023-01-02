#include <amxmodx>
#include <reapi>
#include <json>
#include <VipModular>
#include "VipM/Utils"
#include "VipM/DebugMode"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][I] Default";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "[VipModular][Item] Default items.";

new Float:g_fSpeedMult[MAX_PLAYERS + 1] = {1.0, ...};

public VipM_IC_OnInitTypes() {
    RegisterPluginByVars();

    VipM_IC_RegisterType("Random");
    VipM_IC_RegisterTypeEvent("Random", ItemType_OnRead, "@OnRandomRead");
    VipM_IC_RegisterTypeEvent("Random", ItemType_OnGive, "@OnRandomGive");

    VipM_IC_RegisterType("InstantReloadAllWeapons");
    VipM_IC_RegisterTypeEvent("InstantReloadAllWeapons", ItemType_OnGive, "@OnInstantReloadAllWeaponsGive");

    VipM_IC_RegisterType("InstantReload");
    VipM_IC_RegisterTypeEvent("InstantReload", ItemType_OnGive, "@OnInstantReloadGive");

    VipM_IC_RegisterType("RefillBpAmmo");
    VipM_IC_RegisterTypeEvent("RefillBpAmmo", ItemType_OnGive, "@OnRefillBpAmmoGive");

    VipM_IC_RegisterType("Speed");
    VipM_IC_RegisterTypeEvent("Speed", ItemType_OnRead, "@OnSpeedRead");
    VipM_IC_RegisterTypeEvent("Speed", ItemType_OnGive, "@OnSpeedGive");

    VipM_IC_RegisterType("Money");
    VipM_IC_RegisterTypeEvent("Money", ItemType_OnRead, "@OnMoneyRead");
    VipM_IC_RegisterTypeEvent("Money", ItemType_OnGive, "@OnMoneyGive");

    VipM_IC_RegisterType("Weapon");
    VipM_IC_RegisterTypeEvent("Weapon", ItemType_OnRead, "@OnWeaponRead");
    VipM_IC_RegisterTypeEvent("Weapon", ItemType_OnGive, "@OnWeaponGive");

    VipM_IC_RegisterType("ItemsList");
    VipM_IC_RegisterTypeEvent("ItemsList", ItemType_OnRead, "@OnItemsListRead");
    VipM_IC_RegisterTypeEvent("ItemsList", ItemType_OnGive, "@OnItemsListGive");

    VipM_IC_RegisterType("Command");
    VipM_IC_RegisterTypeEvent("Command", ItemType_OnRead, "@OnCommandRead");
    VipM_IC_RegisterTypeEvent("Command", ItemType_OnGive, "@OnCommandGive");

    VipM_IC_RegisterType("DefuseKit");
    VipM_IC_RegisterTypeEvent("DefuseKit", ItemType_OnGive, "@OnDefuseKitGive");
}

@OnPlayerSpawnPre(const UserId) {
    g_fSpeedMult[UserId] = 1.0;
}

@OnPlayerResetSpeedPost(const UserId) {
    if (g_fSpeedMult[UserId] != 1.0) {
        MultUserSpeed(UserId, g_fSpeedMult[UserId]);
    }
}

@OnRandomRead(const JSON:jItem, const Trie:tParams) {
    TrieDeleteKey(tParams, "Name");

    // TODO: Сделать как-то разные шансы
    if (!json_object_has_value(jItem, "Items")) {
        Json_LogForFile(jItem, "ERROR", "Param `Items` required for item `Random`.");
        return VIPM_STOP;
    }
    
    new Array:aItems = VipM_IC_JsonGetItems(json_object_get_value(jItem, "Items"));
    if (ArraySizeSafe(aItems) <= 1) {
        Json_LogForFile(jItem, "ERROR", "Param `Items` must have >1 items.");
        ArrayDestroy(aItems);
        return VIPM_STOP;
    }

    TrieSetCell(tParams, "Items", aItems);

    return VIPM_CONTINUE;
}

@OnRandomGive(const UserId, const Trie:tParams) {
    new Array:aItems = VipM_Params_GetArr(tParams, "Items");
    new iRandomIndex = random_num(0, ArraySizeSafe(aItems) - 1);
    new VipM_IC_T_Item:iRandomItem = ArrayGetCell(aItems, iRandomIndex);

    return VipM_IC_GiveItem(UserId, iRandomItem) ? VIPM_CONTINUE : VIPM_STOP;
}

@OnInstantReloadGive(const UserId, const Trie:Params) {
    InstantReloadActiveWeapon(UserId);

    return VIPM_CONTINUE;
}

@OnInstantReloadAllWeaponsGive(const UserId, const Trie:Params) {
    InstantReloadAllWeapons(UserId);

    return VIPM_CONTINUE;
}

@OnRefillBpAmmoGive(const UserId, const Trie:Params) {
    new iMaxAmmos[32] = {-1, ...};

    for (new WeaponIdType:iWpnId = WEAPON_P228; iWpnId < WEAPON_P90; iWpnId++) {
        iMaxAmmos[rg_get_weapon_info(iWpnId, WI_AMMO_TYPE)] = rg_get_weapon_info(iWpnId, WI_MAX_ROUNDS);
    }

    for (new InventorySlotType:iSlot = PRIMARY_WEAPON_SLOT; iSlot <= PISTOL_SLOT; iSlot++) {
        new ItemId = get_member(UserId, m_rgpPlayerItems, iSlot);
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
            set_member(UserId, m_rgAmmo, iMaxAmmos[iAmmoType], iAmmoType);
        }
    }

    return VIPM_CONTINUE;
}

@OnSpeedRead(const JSON:jItem, const Trie:Params) {
    TrieDeleteKey(Params, "Name");

    if (!json_object_has_value(jItem, "Multiplier", JSONNumber)) {
        Json_LogForFile(jItem, "ERROR", "Param `Multiplier` required for item `Speed`.");
        return VIPM_STOP;
    }
    TrieSetCell(Params, "Multiplier", json_object_get_real(jItem, "Multiplier"));

    static bIsUsed;
    if (!bIsUsed) {
        bIsUsed = true;
        RegisterHookChain(RG_CBasePlayer_Spawn, "@OnPlayerSpawnPre", false);
        RegisterHookChain(RG_CBasePlayer_ResetMaxSpeed, "@OnPlayerResetSpeedPost", true);
    }

    return VIPM_CONTINUE;
}

@OnSpeedGive(const UserId, const Trie:Params) {
    g_fSpeedMult[UserId] = VipM_Params_GetFloat(Params, "Multiplier", 1.0);
    MultUserSpeed(UserId, g_fSpeedMult[UserId]);

    return VIPM_CONTINUE;
}

@OnMoneyRead(const JSON:jItem, const Trie:Params) {
    TrieDeleteKey(Params, "Name");

    if (!json_object_has_value(jItem, "Amount", JSONNumber)) {
        Json_LogForFile(jItem, "ERROR", "Param `Amount` required for item `Money`.");
        return VIPM_STOP;
    }
    TrieSetCell(Params, "Amount", json_object_get_number(jItem, "Amount"));

    if (json_object_has_value(jItem, "GiveType", JSONString)) {
        new sBuffer[32];
        json_object_get_string(jItem, "GiveType", sBuffer, charsmax(sBuffer));
        new accountSet = StrToAccountSet(sBuffer);
        if (accountSet < 0) {
            Json_LogForFile(jItem, "ERROR", "Invalid `GiveType` value (%s). Expected `Set` or `Add`.", sBuffer);
        } else {
            TrieSetCell(Params, "GiveType", accountSet);
        }
    }

    if (json_object_has_value(jItem, "TrackChange", JSONBoolean)) {
        TrieSetCell(Params, "TrackChange", json_object_get_bool(jItem, "TrackChange"));
    }

    return VIPM_CONTINUE;
}

@OnMoneyGive(const UserId, const Trie:Params) {
    rg_add_account(
        UserId,
        VipM_Params_GetInt(Params, "Amount"),
        VipM_Params_GetCell(Params, "GiveType", AS_ADD),
        VipM_Params_GetBool(Params, "TrackChanges", true)
    );
    return VIPM_CONTINUE;
}

@OnItemsListRead(const JSON:jItem, const Trie:Params) {
    TrieDeleteKey(Params, "Name");

    if (!json_object_has_value(jItem, "Items")) {
        Json_LogForFile(jItem, "ERROR", "Param `Items` required for item `ItemsList`.");
        return VIPM_STOP;
    }

    TrieSetCell(Params, "Items", VipM_IC_JsonGetItems(json_object_get_value(jItem, "Items")));

    return VIPM_CONTINUE;
}

@OnItemsListGive(const UserId, const Trie:Params) {
    new Array:aItems = VipM_Params_GetCell(Params, "Items", Invalid_Array);

    return VipM_IC_GiveItems(UserId, aItems) ? VIPM_CONTINUE : VIPM_STOP;
}

@OnCommandRead(const JSON:jItem, const Trie:Params) {
    TrieDeleteKey(Params, "Name");

    if (!json_object_has_value(jItem, "Command", JSONString)) {
        Json_LogForFile(jItem, "ERROR", "Param `Command` required for item `Command`.");
        return VIPM_STOP;
    }
    
    new Command[128];
    json_object_get_string(jItem, "Command", Command, charsmax(Command));
    TrieSetString(Params, "Command", Command);

    if (json_object_has_value(jItem, "ByServer", JSONBoolean)) {
        TrieSetCell(Params, "ByServer", json_object_get_bool(jItem, "ByServer"));
    }

    return VIPM_CONTINUE;
}

@OnCommandGive(const UserId, const Trie:Params) {
    static Command[128];
    VipM_Params_GetStr(Params, "Command", Command, charsmax(Command));
    new bool:ByServer = VipM_Params_GetBool(Params, "ByServer", false);

    replace_all(Command, charsmax(Command), "{UserId}", IntToStr(UserId));

    if (ByServer) {
        server_cmd(Command);
    } else {
        client_cmd(UserId, Command);
    }
}

@OnDefuseKitGive(const UserId, const Trie:Params) {
    if (get_member(UserId, m_iTeam) == TEAM_CT) {
        rg_give_defusekit(UserId);
    }

    return VIPM_CONTINUE;
}

@OnWeaponRead(const JSON:jItem, const Trie:Params) {
    new Name[32];
    json_object_get_string(jItem, "Name", Name, charsmax(Name));

    // if(get_weaponid(Name) == 0){
    //     log_amx("[WARNING] Weapon `%s` not found.", Name);
    //     return VIPM_STOP;
    // }

    TrieSetCell(Params, "GiveType", _:Json_Object_GetGiveType(jItem, "GiveType"));

    if (json_object_has_value(jItem, "BpAmmo", JSONNumber)) {
        TrieSetCell(Params, "BpAmmo", json_object_get_number(jItem, "BpAmmo"));
    }

    TrieSetString(Params, "Name", Name);

    return VIPM_CONTINUE;
}

@OnWeaponGive(const UserId, const Trie:Params) {
    static WeaponName[32];
    VipM_Params_GetStr(Params, "Name", WeaponName, charsmax(WeaponName));
    if (!WeaponName[0]) {
        return VIPM_STOP;
    }
    
    if (!get_weaponid(WeaponName)) {
        log_amx("[WARNING] Default weapon `%s` not found.", WeaponName);
        return VIPM_STOP;
    }
    
    new GiveType:iGiveType = GiveType:VipM_Params_GetInt(Params, "GiveType", _:GT_DROP_AND_REPLACE);
    new iBpAmmo = VipM_Params_GetInt(Params, "BpAmmo", -1);

    new ItemId = rg_give_item(UserId, WeaponName, iGiveType);
    if (ItemId < 0) {
        return VIPM_STOP;
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
        return VIPM_CONTINUE;
    }

    new def_BpAmmo = (rg_get_weapon_info(iWpnId, WI_MAX_ROUNDS));
    rg_set_user_bpammo(UserId, iWpnId, iBpAmmo < 0 ? def_BpAmmo : iBpAmmo);

    return VIPM_CONTINUE;
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

MultUserSpeed(const UserId, const Float:fMultiplier) {
    set_entvar(UserId, var_maxspeed, Float:get_entvar(UserId, var_maxspeed) * fMultiplier);
}
