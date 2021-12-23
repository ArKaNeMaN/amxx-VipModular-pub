#include <amxmodx>
#include <VipModular>
#include <TimeOfDay>

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][L] Time Of Day";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "[VipModular] Access by day time.";

new const LIMIT_TYPE_NAME[] = "ToD-DayTime";

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);

    VipM_Limits_RegisterType(LIMIT_TYPE_NAME, false, false);
    VipM_Limits_AddTypeParams(LIMIT_TYPE_NAME,
        "DayTime", ptString, true
    );
    VipM_Limits_RegisterTypeEvent(LIMIT_TYPE_NAME, Limit_OnCheck, "@OnDayTimeCheck");
}

new g_iLastTime = 0;
new g_sLastDayTime[32];
const DAYTIME_CACHE_LIFETIME = 60;

UpdateDayTimeCache(){
    if(get_systime() - g_iLastTime <= DAYTIME_CACHE_LIFETIME)
        return;

    ToD_GetTimeName(g_sLastDayTime, charsmax(g_sLastDayTime));
    g_iLastTime = get_systime();
}

@OnDayTimeCheck(const UserId, const Trie:Params){
    static DayTime[32];
    VipM_Params_GetStr(Params, "DayTime", DayTime, charsmax(DayTime));

    UpdateDayTimeCache();

    return bool:equali(g_sLastDayTime, DayTime);
}
