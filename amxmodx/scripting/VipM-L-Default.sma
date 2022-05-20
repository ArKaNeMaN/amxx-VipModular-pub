#include amxmodx
#include reapi
#include VipModular

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][L] Default";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";

// bool:HasAllBits(const iBitSum1, const iBitSumm2)
#define HasAllBits(%1,%2) \
    bool:(%1 & %2 == %2)

// bool:HasAllBits(const iBitSum1, const iBitSumm2)
#define HasAllBits(%1,%2) \
    bool:(%1 & %2 == %2)

// bool:HasUserFlags(const UserId, const iFlags)
#define HasUserFlags(%1,%2) \
    HasBits(get_user_flags(%1), %2)

// bool:HasUserFlagsS(const UserId, const sFlags[])
#define HasUserFlagsS(%1,%2) \
    HasUserFlags(%1, read_flags(%2))

// []GetUserName(const UserId)
#define GetUserName(%1) \
    fmt("%n",%1)

// bool:IsEqualUserName(const UserId, const sName[])
#define IsEqualUserName(%1,%2) \
    equal(GetUserName(%1), %2)

// _:GetRound()
#define GetRound() \
    get_member_game(m_iTotalRoundsPlayed) + 1

new g_sSteamIds[MAX_PLAYERS + 1][64];
new g_sIps[MAX_PLAYERS + 1][32];
new g_sRealMapName[32];

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);

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
        "Real", ptBoolean, false
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
}

public client_authorized(UserId, const AuthId[]){
    VipM_Limits_SetStaticValue("Steam", is_user_steam(UserId), UserId);
    VipM_Limits_SetStaticValue("Bot", bool:is_user_bot(UserId), UserId);

    copy(g_sSteamIds[UserId], charsmax(g_sSteamIds[]), AuthId);
    get_user_ip(UserId, g_sIps[UserId], charsmax(g_sIps[]), true);
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

GetWeekDayIdByName(const sWeekDayName[]) {
    if (equali("Вс", sWeekDayName) || equali("Воскресенье", sWeekDayName) || equali("Sun", sWeekDayName) || equali("Sunday", sWeekDayName)) {
        return 0;
    } else if (equali("Пн", sWeekDayName) || equali("Понедельник", sWeekDayName) || equali("Mon", sWeekDayName) || equali("Monday", sWeekDayName)) {
        return 1;
    } else if (equali("Вт", sWeekDayName) || equali("Вторник", sWeekDayName) || equali("Tue", sWeekDayName) || equali("Tuesday", sWeekDayName)) {
        return 2;
    } else if (equali("Ср", sWeekDayName) || equali("Среда", sWeekDayName) || equali("Wed", sWeekDayName) || equali("Wednesday", sWeekDayName)) {
        return 3;
    } else if (equali("Чт", sWeekDayName) || equali("Четверг", sWeekDayName) || equali("Thu", sWeekDayName) || equali("Thursday", sWeekDayName)) {
        return 4;
    } else if (equali("Пт", sWeekDayName) || equali("Пятница", sWeekDayName) || equali("Fri", sWeekDayName) || equali("Friday", sWeekDayName)) {
        return 5;
    } else if (equali("Сб", sWeekDayName) || equali("Суббота", sWeekDayName) || equali("Sat", sWeekDayName) || equali("Saturday", sWeekDayName)) {
        return 6;
    } else {
        return -1;
    }
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
    new iFlags = read_flags(sFlags);

    new bitwiseAndRes = get_user_flags(UserId) & iFlags;

    return VipM_Params_GetBool(Params, "Strict", false)
        ? bitwiseAndRes == iFlags
        : bitwiseAndRes > 0;
}

@OnSteamIdCheck(const Trie:Params, const UserId) {
    static sSteamId[64];
    VipM_Params_GetStr(Params, "SteamId", sSteamId, charsmax(sSteamId));

    return equal(g_sSteamIds[UserId], sSteamId);
}

@OnIpCheck(const Trie:Params, const UserId) {
    static sIp[32];
    VipM_Params_GetStr(Params, "SteamId", sIp, charsmax(sIp));

    return equal(g_sIps[UserId], sIp);
}

@OnMapCheck(const Trie:Params) {
    static sMap[32], bool:bReal = false;
    VipM_Params_GetStr(Params, "Map", sMap, charsmax(sMap));
    bReal = VipM_Params_GetBool(Params, "Real", false);

    if (bReal) {
        return equali(sMap, g_sRealMapName);
    } else {
        static sSetMapName[32];
        rh_get_mapname(sSetMapName, charsmax(sSetMapName), MNT_SET);
        return equali(sMap, sSetMapName);
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
