#include <amxmodx>
#include <VipModular>
#include <TimeOfDay>
#include "VipM/DebugMode"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][L] Time Of Day";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "[VipModular] Access by day time.";

new const LIMIT_TYPE_NAME[] = "ToD-DayTime";
new const DAYTIME_PARAM_NAME[] = "DayTime";

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);

    VipM_Limits_RegisterType(LIMIT_TYPE_NAME, false, false);
    VipM_Limits_AddTypeParams(LIMIT_TYPE_NAME,
        DAYTIME_PARAM_NAME, ptString, true
    );
    VipM_Limits_RegisterTypeEvent(LIMIT_TYPE_NAME, Limit_OnCheck, "@OnDayTimeCheck");
}

new g_iLastTime = 0;
new g_sLastDayTime[32];
new const DAYTIME_CACHE_LIFETIME = 60;

UpdateDayTimeCache() {
    if (get_systime() - g_iLastTime <= DAYTIME_CACHE_LIFETIME) {
        return; 
    }

    ToD_GetTimeName(g_sLastDayTime, charsmax(g_sLastDayTime));
    g_iLastTime = get_systime();

    Dbg_Log("UpdateDayTimeCache(): %s, %d", g_sLastDayTime, g_iLastTime);
}

@OnDayTimeCheck(const Trie:Params, const UserId) {
    new sDayTime[32];
    Dbg_Log("@OnDayTimeCheck(...): Current = %s", TrieGetString(Params, DAYTIME_PARAM_NAME, sDayTime, charsmax(sDayTime)) ? sDayTime : NULL_STRING);

    new DayTime[32];
    VipM_Params_GetStr(Params, DAYTIME_PARAM_NAME, DayTime, charsmax(DayTime));

    UpdateDayTimeCache();
    Dbg_Log("@OnDayTimeCheck(#%d): (%s == %s)", UserId, g_sLastDayTime, DayTime);

    return bool:equali(g_sLastDayTime, DayTime);
}
