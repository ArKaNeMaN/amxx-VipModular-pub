#include <amxmodx>
#include <reapi>
#include <json>
#include <VipModular>

stock const __NUM_STR[] = "%d";
#define IntToStr(%1) \
    fmt(__NUM_STR, %1)

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][I] Default";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "[VipModular][Item] Default items.";

public VipM_IC_OnInitTypes() {
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);

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

@OnItemsListRead(const JSON:jItem, const Trie:Params) {
    TrieDeleteKey(Params, "Name");

    if (!json_object_has_value(jItem, "Items", JSONArray)) {
        log_amx("[WARNING] Param `Items` required for item `ItemsList`.");
        return VIPM_STOP;
    }

    new JSON:jItems = json_object_get_value(jItem, "Items");
    TrieSetCell(Params, "Items", VipM_IC_JsonGetItems(jItems));

    return VIPM_CONTINUE;
}

@OnItemsListGive(const UserId, const Trie:Params) {
    new Array:aItems = VipM_Params_GetCell(Params, "Items", Invalid_Array);

    return VipM_IC_GiveItems(UserId, aItems) ? VIPM_CONTINUE : VIPM_STOP;
}

@OnCommandRead(const JSON:jItem, const Trie:Params) {
    TrieDeleteKey(Params, "Name");

    if (!json_object_has_value(jItem, "Command")) {
        log_amx("[WARNING] Param `Command` required for item `Command`.");
        return VIPM_STOP;
    }
    
    new Command[128];
    json_object_get_string(jItem, "Command", Command, charsmax(Command));
    TrieSetString(Params, "Command", Command);

    TrieSetCell(Params, "ByServer", json_object_get_bool(jItem, "ByServer"));

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

    // if(get_weaponid(Name)  == 0){
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
