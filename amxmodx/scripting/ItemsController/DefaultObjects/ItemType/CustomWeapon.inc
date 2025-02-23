#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

#if !defined _cwapi_included
enum T_CustomWeapon { Invalid_CustomWeapon = -1 }
enum CWeapon_GiveType {
    CWAPI_GT_SMART = -1,
    CWAPI_GT_APPEND,
    CWAPI_GT_REPLACE,
    CWAPI_GT_DROP,
}
native T_CustomWeapon:CWAPI_Weapons_Find(const sWeaponName[]);
native CWAPI_Weapons_Give(
    const UserId,
    const T_CustomWeapon:iWeapon,
    const CWeapon_GiveType:iGiveType = CWAPI_GT_SMART,
    const bool:bForceActive = false,
    const initBpAmmo = -1
);
#endif

#if !defined _auw_included
native __auw__native__auw_give_weapon(iPlayer, const sWeaponName[], const bool: notification = true, const uid = -1, const iUidWithOffset = true, const iBuy = 0);
#endif

#if !defined _ultimate_weapons_included
native weapons_give_user_ultimate(id, uid=-1, buy_name[]="", replace=0, ammo=-1, bpammo=-1);
#endif

enum E_ItemType_CustomWeapon_Controller {
    ItemType_CustomWeapon_Controller_None,

    ItemType_CustomWeapon_Controller_Cwapi,
    ItemType_CustomWeapon_Controller_Auw,
    ItemType_CustomWeapon_Controller_Uw,
}

enum E_ItemType_CustomWeapon_GiveType {
    ItemType_CustomWeapon_GiveType_Default,
    ItemType_CustomWeapon_GiveType_Append,
    ItemType_CustomWeapon_GiveType_Replace,
    ItemType_CustomWeapon_GiveType_DropAndReplace,
}

static E_ItemType_CustomWeapon_Controller:UsingController = ItemType_CustomWeapon_Controller_None;
static bool:AvailableControllers[E_ItemType_CustomWeapon_Controller] = {false, ...};

bool:DefaultObjects_ItemType_CustomWeapon_NativeFilter(const name[], const trap) {
    if (equal(name, "CWAPI_Weapons_Give")) {
        AvailableControllers[ItemType_CustomWeapon_Controller_Cwapi] = !!trap;
        return true;
    }
    if (equal(name, "CWAPI_Weapons_Find")) {
        return true;
    }

    if (equal(name, "__auw__native__auw_give_weapon")) {
        AvailableControllers[ItemType_CustomWeapon_Controller_Auw] = !!trap;
        return true;
    }

    if (equal(name, "weapons_give_user_ultimate")) {
        AvailableControllers[ItemType_CustomWeapon_Controller_Uw] = !!trap;
        return true;
    }

    return false;
}

DefaultObjects_ItemType_CustomWeapon_Register() {
    UsingController = ItemType_CustomWeapon_Controller_None;
    for (new i = 0; i < _:E_ItemType_CustomWeapon_Controller; ++i) {
        if (AvailableControllers[E_ItemType_CustomWeapon_Controller:i]) {
            UsingController = E_ItemType_CustomWeapon_Controller:i;
        }
    }

    IC_ItemType_SimpleRegister(
        .name = "CustomWeapon",
        .onRead = "@ItemType_CustomWeapon_OnRead",
        .onGive = "@ItemType_CustomWeapon_OnGive"
    );
}

@ItemType_CustomWeapon_OnRead(const JSON:instanceJson, const Trie:p) {
    if (UsingController == ItemType_CustomWeapon_Controller_None) {
        log_amx("[WARNING] Custom weapons controller is not set. Item type `CustomWeapon` is not available.");
        return IC_RET_READ_FAIL;
    }

    new Name[32];
    json_object_get_string(instanceJson, "Name", Name, charsmax(Name));
    TrieSetString(p, "Name", Name);

    TrieSetCell(p, "GiveType", _:ItemType_CustomWeapon_ReadGiveTypeFromJsonObject(instanceJson, "GiveType"));

    return IC_RET_READ_SUCCESS;
}

@ItemType_CustomWeapon_OnGive(const playerIndex, const Trie:p) {
    if (UsingController == ItemType_CustomWeapon_Controller_None) {
        log_amx("[ERROR] Custom weapons controller is not set.");
        return IC_RET_GIVE_FAIL;
    }

    static name[32];
    VipM_Params_GetStr(p, "Name", name, charsmax(name));
    if (!name[0]) {
        log_amx("[WARNING] Custom weapon name is empty.");
        return IC_RET_GIVE_FAIL;
    }
    
    new E_ItemType_CustomWeapon_GiveType:giveType = E_ItemType_CustomWeapon_GiveType:VipM_Params_GetInt(p, "GiveType", _:ItemType_CustomWeapon_GiveType_Default);

    new bool:res = false;
    switch (UsingController) {
        case ItemType_CustomWeapon_Controller_Cwapi:
            res = ItemType_CustomWeapon_GiveViaCwapi(playerIndex, name, giveType);
        case ItemType_CustomWeapon_Controller_Auw:
            res = ItemType_CustomWeapon_GiveViaAuw(playerIndex, name);
        case ItemType_CustomWeapon_Controller_Uw:
            res = ItemType_CustomWeapon_GiveViaUw(playerIndex, name, giveType);
        default:
            log_amx("[WARNING] Unknown custom weapons controller.");
    }

    return res ? IC_RET_GIVE_SUCCESS : IC_RET_GIVE_FAIL;
}

static bool:ItemType_CustomWeapon_GiveViaCwapi(
    const playerIndex,
    const weaponName[],
    const E_ItemType_CustomWeapon_GiveType:giveType = ItemType_CustomWeapon_GiveType_Default
) {
    new CWeapon_GiveType:gtype;
    switch (giveType) {
        case ItemType_CustomWeapon_GiveType_Append:
            gtype = CWAPI_GT_APPEND;
        case ItemType_CustomWeapon_GiveType_Replace:
            gtype = CWAPI_GT_REPLACE;
        case ItemType_CustomWeapon_GiveType_DropAndReplace:
            gtype = CWAPI_GT_DROP;
        default:
            gtype = CWAPI_GT_SMART;
    }

    new T_CustomWeapon:weapon = CWAPI_Weapons_Find(weaponName);
    if (weapon == Invalid_CustomWeapon) {
        abort(AMX_ERR_PARAMS, "Weapon '%s' not found in Custom Wepaons API.", weaponName);
        return false;
    }
    
    return CWAPI_Weapons_Give(playerIndex, weapon, gtype) > 0;
}

static bool:ItemType_CustomWeapon_GiveViaAuw(
    const playerIndex,
    const weaponName[]
) {
    return !!__auw__native__auw_give_weapon(playerIndex, weaponName);
}

static bool:ItemType_CustomWeapon_GiveViaUw(
    const playerIndex,
    const weaponName[],
    const E_ItemType_CustomWeapon_GiveType:giveType = ItemType_CustomWeapon_GiveType_Default
) {
    new replace;
    switch (giveType) {
        case ItemType_CustomWeapon_GiveType_Append:
            replace = 0;
        case ItemType_CustomWeapon_GiveType_Replace:
            replace = 2;
        case ItemType_CustomWeapon_GiveType_DropAndReplace:
            replace = 1;
        default:
            replace = 0;
    }

    new nonConstName[32];
    copy(nonConstName, charsmax(nonConstName), weaponName);

    // Не понятно что оно возвращает, поэтому в любом случае будет успех.
    weapons_give_user_ultimate(
        playerIndex,
        .buy_name = nonConstName,
        .replace = replace
    );

    return true;
}

static E_ItemType_CustomWeapon_GiveType:ItemType_CustomWeapon_ReadGiveTypeFromJsonObject(const JSON:Obj, const Key[], const bool:DotNot = false) {
    new Str[32];
    json_object_get_string(Obj, Key, Str, charsmax(Str), DotNot);
    
    if (equali(Str, "GT_APPEND") || equali(Str, "Append") || equali(Str, "Add")) {
        return ItemType_CustomWeapon_GiveType_Append;
    } else if (equali(Str, "GT_REPLACE") || equali(Str, "Replace")) {
        return ItemType_CustomWeapon_GiveType_Replace;
    } else if (equali(Str, "GT_DROP_AND_REPLACE") || equali(Str, "Drop") || equali(Str, "DropAndReplace")) {
        return ItemType_CustomWeapon_GiveType_DropAndReplace;
    } else {
        return ItemType_CustomWeapon_GiveType_Default;
    }
}
