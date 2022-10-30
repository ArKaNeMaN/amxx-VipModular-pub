#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][L] Default";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;

new g_sSteamIds[MAX_PLAYERS + 1][64];
new g_sIps[MAX_PLAYERS + 1][32];
new g_sRealMapName[32];

public VipM_OnInitModules(){
    RegisterPluginByVars();

    VipM_Limits_RegisterType("ForAll", false, true);
    VipM_Limits_SetStaticValue("ForAll", true);
    VipM_Limits_RegisterType("Always", false, true);
    VipM_Limits_SetStaticValue("Always", true);

    VipM_Limits_RegisterType("Never", false, true);
    VipM_Limits_SetStaticValue("Never", false);

    VipM_Limits_RegisterType("Steam", true, true);

    VipM_Limits_RegisterType("Alive", true, false);
    VipM_Limits_RegisterTypeEvent("Alive", Limit_OnCheck, "@OnAliveCheck");

    VipM_Limits_RegisterType("Bot", true, true);

    VipM_Limits_RegisterType("Name", true, false);
    VipM_Limits_AddTypeParams("Name",
        "Name", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("Name", Limit_OnCheck, "@OnNameCheck");

    VipM_Limits_RegisterType("Flags", true, false);
    VipM_Limits_AddTypeParams("Flags",
        "Flags", ptString, true,
        "Strict", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("Flags", Limit_OnCheck, "@OnFlagsCheck");

    VipM_Limits_RegisterType("SteamId", true, false);
    VipM_Limits_AddTypeParams("SteamId",
        "SteamId", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("SteamId", Limit_OnCheck, "@OnSteamIdCheck");

    VipM_Limits_RegisterType("Ip", true, false);
    VipM_Limits_AddTypeParams("Ip",
        "Ip", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("Ip", Limit_OnCheck, "@OnIpCheck");

    VipM_Limits_RegisterType("Map", false, false);
    VipM_Limits_AddTypeParams("Map",
        "Map", ptString, true,
        "Real", ptBoolean, false,
        "Prefix", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("Map", Limit_OnCheck, "@OnMapCheck");
    rh_get_mapname(g_sRealMapName, charsmax(g_sRealMapName), MNT_TRUE);

    VipM_Limits_RegisterType("HasPrimaryWeapon", true, false);
    VipM_Limits_AddTypeParams("HasPrimaryWeapon",
        "HasNot", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("HasPrimaryWeapon", Limit_OnCheck, "@OnHasPrimaryWeaponCheck");

    VipM_Limits_RegisterType("Round", false, false);
    VipM_Limits_AddTypeParams("Round",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("Round", Limit_OnCheck, "@OnRoundCheck");

    VipM_Limits_RegisterType("WeekDay", false, false);
    VipM_Limits_AddTypeParams("WeekDay",
        "Day", ptString, true
    );
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnRead, "@OnWeekDayRead");
    VipM_Limits_RegisterTypeEvent("WeekDay", Limit_OnCheck, "@OnWeekDayCheck");

    VipM_Limits_RegisterType("RoundTime", false, false);
    VipM_Limits_AddTypeParams("RoundTime",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("RoundTime", Limit_OnCheck, "@OnRoundTimeCheck");

    VipM_Limits_RegisterType("InFreezyTime", false, false);
    VipM_Limits_AddTypeParams("InFreezyTime",
        "Reverse", ptBoolean, false
    );
    VipM_Limits_RegisterTypeEvent("InFreezyTime", Limit_OnCheck, "@OnInFreezyTimeCheck");
}

public client_authorized(UserId, const AuthId[]){
    VipM_Limits_SetStaticValue("Steam", is_user_steam(UserId), UserId);
    VipM_Limits_SetStaticValue("Bot", bool:is_user_bot(UserId), UserId);

    copy(g_sSteamIds[UserId], charsmax(g_sSteamIds[]), AuthId);
    get_user_ip(UserId, g_sIps[UserId], charsmax(g_sIps[]), true);
}

@OnInFreezyTimeCheck(const Trie:Params) {
    new bool:bFreezyPeriod = get_member_game(m_bFreezePeriod);
    return VipM_Params_GetBool(Params, "Reverse", false) ? !bFreezyPeriod : bFreezyPeriod;
}

@OnRoundTimeCheck(const Trie:Params) {
    new iMin = VipM_Params_GetInt(Params, "Min", 0);
    new iMax = VipM_Params_GetInt(Params, "Max", 0);
    new iRoundTime = floatround(get_member_game(m_iRoundTime), floatround_floor);

    return (
        (!iMin || iRoundTime >= iMin)
        && (!iMax || iRoundTime <= iMax)
    );
}

@OnWeekDayRead(const JSON:jCfg, const Trie:Params) {
    new sWeekDayName[32];
    json_object_get_string(jCfg, "Day", sWeekDayName, charsmax(sWeekDayName));
    new iWeekDayIndex = GetWeekDayIdByName(sWeekDayName);
    if (iWeekDayIndex < 0) {
        log_amx("[WARNING] Undefined week day '%s'.", sWeekDayName);
        return VIPM_STOP;
    }

    TrieSetCell(Params, "Day", iWeekDayIndex);
    return VIPM_CONTINUE;
}

@OnWeekDayCheck(const Trie:Params) {
    new sWeekDay[4];
    get_time("%w", sWeekDay, charsmax(sWeekDay));
    return str_to_num(sWeekDay) == VipM_Params_GetInt(Params, "Day", -1);
}

@OnAliveCheck(const Trie:Params, const UserId) {
    return is_user_alive(UserId);
}

@OnNameCheck(const Trie:Params, const UserId) {
    static sName[32];
    VipM_Params_GetStr(Params, "Name", sName, charsmax(sName));

    return IsEqualUserName(UserId, sName);
}

bool:@OnFlagsCheck(const Trie:Params, const UserId) {
    static sFlags[16];
    VipM_Params_GetStr(Params, "Flags", sFlags, charsmax(sFlags));

    return HasUserFlagsStr(UserId, sFlags, VipM_Params_GetBool(Params, "Strict", false));
}

@OnSteamIdCheck(const Trie:Params, const UserId) {
    static sSteamId[64];
    VipM_Params_GetStr(Params, "SteamId", sSteamId, charsmax(sSteamId));

    return equali(g_sSteamIds[UserId], sSteamId);
}

@OnIpCheck(const Trie:Params, const UserId) {
    static sIp[32];
    VipM_Params_GetStr(Params, "SteamId", sIp, charsmax(sIp));

    return equali(g_sIps[UserId], sIp);
}

@OnMapCheck(const Trie:Params) {
    static sMap[32];
    VipM_Params_GetStr(Params, "Map", sMap, charsmax(sMap));

    new iCount = VipM_Params_GetBool(Params, "Prefix", false) ? strlen(sMap) : 0;

    if (VipM_Params_GetBool(Params, "Real", false)) {
        return equali(sMap, g_sRealMapName, iCount);
    } else {
        static sSetMapName[32];
        rh_get_mapname(sSetMapName, charsmax(sSetMapName), MNT_SET);
        return equali(sMap, sSetMapName, iCount);
    }
}

@OnHasPrimaryWeaponCheck(const Trie:Params, const UserId) {
    new bool:res = get_member(UserId, m_bHasPrimary);
    return VipM_Params_GetBool(Params, "HasNot", false) ? !res : res;
}

@OnRoundCheck(const Trie:Params, const UserId){
    return (
        GetRound() >= VipM_Params_GetInt(Params, "Min", -1)
        && GetRound() <= VipM_Params_GetInt(Params, "Max", cellmax)
    );
}
