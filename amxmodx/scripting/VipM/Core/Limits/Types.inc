#include amxmodx
#include "VipM/ArrayMap"
#include "VipM/Utils"

enum T_Limit {Invalid_Limit = -1}

enum _:S_Limit{
    Limit_PluginId,
    Limit_Name[32],

    bool:Limit_Static,
    Limit_StaticValue,

    bool:Limit_ForPlayer,

    Array:Limit_Params,
    Trie:Limit_Events,
}

new ArrayMap(Limits);

#define LIMIT_TYPE_PUSH(%1) \
    T_Limit:ArrayMapPushArray(Limits, %1, %1[Limit_Name])

#define LIMIT_TYPE_EXISTS(%1) \
    ArrayMapHasKey(Limits, %1)

#define LIMIT_TYPE_GET_ID(%1) \
    T_Limit:ArrayMapGetIndex(Limits, %1)

#define LIMIT_TYPE_GET_BY_ID(%1,%2) \
    ArrayMapGetiArray(Limits, _:(%1), %2)

#define LIMIT_TYPE_GET(%1,%2) \
    ArrayMapGetArray(Limits, %1, %2)

#define SET_LIMIT_TYPE(%1) \
    ArrayMapSetArray(Limits, %1[Limit_Name], %1)

new const __CHECK_LIMIT_FOR_PLAYER_errmsg[] = "[ERROR] Limit `%s` must used for player.";
#define CHECK_LIMIT_FOR_PLAYER(%1,%2,%3) CompositeMacros( \
    if( \
        !IsUserIdValid(%2) \
        && %1[Limit_ForPlayer] \
    ){ \
        log_error(1, __CHECK_LIMIT_FOR_PLAYER_errmsg, %1[Limit_Name]); \
        return %3; \
    } \
)

new const __CHECK_LIMIT_STATIC_errmsg[] = "[ERROR] Limit type `%s` isn`t static.";
#define CHECK_LIMIT_STATIC(%1,%2) \
    if(!%1[Limit_Static]){ \
        log_error(1, __CHECK_LIMIT_STATIC_errmsg, %1[Limit_Name]); \
        return %2; \
    }

// EMIT_LIMIT_TYPE_EVENT(LimitType, E_LimitEvent:Event, Return, ...Params)
#define EMIT_LIMIT_TYPE_EVENT(%1,%2,%3) \
    if(\
        %1[Limit_Events] != Invalid_Trie\
        && TrieKeyExists(%1[Limit_Events], IntToStr(%2))\
    ){\
        new ___EVENT_FWD;\
        TrieGetCell(%1[Limit_Events], IntToStr(%2), ___EVENT_FWD);\
        ExecuteForward(___EVENT_FWD, %3);\
    }

// SET_LIMIT_TYPE_EVENT(AccessMode, E_LimitEvent:Event, FwdId)
#define SET_LIMIT_TYPE_EVENT(%1,%2,%3) \
    if(%3 >= 0){\
        if(%1[Limit_Events] == Invalid_Trie)\
            %1[Limit_Events] = TrieCreate();\
        TrieSetCell(%1[Limit_Events], IntToStr(%2), %3);\
    }\
    else TrieDeleteKey(%1[Limit_Events], IntToStr(%2))


T_Limit:Limits_AddType(const sName[], const bool:bForPlayer, const bool:bStatic = false, const iPluginId = -1){
    new Limit[S_Limit];

    Limit[Limit_PluginId] = iPluginId;
    copy(Limit[Limit_Name], charsmax(Limit[Limit_Name]), sName);
    Limit[Limit_Static] = bStatic;
    Limit[Limit_StaticValue] = 0;
    Limit[Limit_ForPlayer] = bForPlayer;

    return LIMIT_TYPE_PUSH(Limit);
}

Limits_RegisterEvent(const PluginId, const sLimitName[], const E_LimitEvent:Event, const Func[]){
    new LimitType[S_Limit];
    LIMIT_TYPE_GET(sLimitName, LimitType);

    new FwdId = -1;
    switch(Event){
        case Limit_OnRead: // (const JSON:Cfg, Trie:Params)
            FwdId = CreateOneForward(PluginId, Func, FP_CELL, FP_CELL);

        case Limit_OnCheck: // (const Trie:Params, const UserId)
            FwdId = CreateOneForward(PluginId, Func, FP_CELL, FP_CELL);
    }

    if(FwdId < 0)
        return false;

    SET_LIMIT_TYPE_EVENT(LimitType, Event, FwdId);
    SET_LIMIT_TYPE(LimitType);
    
    return FwdId >= 0;
}

Limits_SetStaticValue(const sLimitName[], const bool:bValue, const UserId = 0){
    static LimitType[S_Limit];
    LIMIT_TYPE_GET(sLimitName, LimitType);

    CHECK_LIMIT_STATIC(LimitType, 0)
    CHECK_LIMIT_FOR_PLAYER(LimitType, UserId, 0);

    if(LimitType[Limit_ForPlayer]){
        if(bValue)
            BitSet(LimitType[Limit_StaticValue], UserId);
        else BitReset(LimitType[Limit_StaticValue], UserId);
    }
    else LimitType[Limit_StaticValue] = _:bValue;
    
    SET_LIMIT_TYPE(LimitType);

    return 0;
}

bool:Limits_GetStaticValue(const sLimitName[], const UserId = 0){
    static LimitType[S_Limit];
    LIMIT_TYPE_GET(sLimitName, LimitType);

    CHECK_LIMIT_STATIC(LimitType, false)
    CHECK_LIMIT_FOR_PLAYER(LimitType, UserId, false);

    return LimitType[Limit_ForPlayer] 
        ? BitIs(LimitType[Limit_StaticValue], UserId) 
        : (bool:LimitType[Limit_StaticValue]);
}

bool:Limits_Execute(const T_Limit:iLimitType, const Trie:Params = Invalid_Trie, const UserId = 0){
    static LimitType[S_Limit];
    LIMIT_TYPE_GET_BY_ID(iLimitType, LimitType);

    CHECK_LIMIT_FOR_PLAYER(LimitType, UserId, false);

    if(LimitType[Limit_Static])
        return Limits_GetStaticValue(LimitType[Limit_Name], UserId);

    new bool:ret;
    EMIT_LIMIT_TYPE_EVENT(LimitType, Limit_OnCheck, ret, Params, UserId)

    return bool:ret;
}
