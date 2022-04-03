#include amxmodx
#include reapi
#include VipModular

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][L] Default";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";

// bool:HasUserFlagsS(const iBitSum1, const iBitSumm2)
#define HasBits(%1,%2) \
    bool:(%1 & %2 == %2)

// bool:HasUserFlagsS(const UserId, const iFlags)
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
        "Flags", ptString, true
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
    VipM_Limits_RegisterTypeEvent("HasPrimaryWeapon", Limit_OnCheck, "@OnHasPrimaryWeaponCheck");

    VipM_Limits_RegisterType("Round", false, false);
    VipM_Limits_AddTypeParams("Round",
        "Min", ptInteger, false,
        "Max", ptInteger, false
    );
    VipM_Limits_RegisterTypeEvent("Round", Limit_OnCheck, "@OnRoundCheck");
}

public client_authorized(UserId, const AuthId[]){
    VipM_Limits_SetStaticValue("Steam", is_user_steam(UserId), UserId);
    VipM_Limits_SetStaticValue("Bot", bool:is_user_bot(UserId), UserId);

    copy(g_sSteamIds[UserId], charsmax(g_sSteamIds[]), AuthId);
    get_user_ip(UserId, g_sIps[UserId], charsmax(g_sIps[]), true);
}

@OnAliveCheck(const Trie:Params, const UserId){
    return is_user_alive(UserId);
}

@OnNameCheck(const Trie:Params, const UserId){
    static sName[32];
    VipM_Params_GetStr(Params, "Name", sName, charsmax(sName));

    return IsEqualUserName(UserId, sName);
}

@OnFlagsCheck(const Trie:Params, const UserId){
    static sFlags[16];
    VipM_Params_GetStr(Params, "Flags", sFlags, charsmax(sFlags));

    return HasUserFlagsS(UserId, sFlags);
}

@OnSteamIdCheck(const Trie:Params, const UserId){
    static sSteamId[64];
    VipM_Params_GetStr(Params, "SteamId", sSteamId, charsmax(sSteamId));

    return equal(g_sSteamIds[UserId], sSteamId);
}

@OnIpCheck(const Trie:Params, const UserId){
    static sIp[32];
    VipM_Params_GetStr(Params, "SteamId", sIp, charsmax(sIp));

    return equal(g_sIps[UserId], sIp);
}

@OnMapCheck(const Trie:Params){
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

@OnHasPrimaryWeaponCheck(const Trie:Params, const UserId){
    return get_member(UserId, m_bHasPrimary);
}

@OnRoundCheck(const Trie:Params, const UserId){
    return (
        GetRound() >= VipM_Params_GetInt(Params, "Min", -1)
        && GetRound() <= VipM_Params_GetInt(Params, "Max", cellmax)
    );
}
