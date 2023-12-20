#if defined __vipm_core_objects_limits_type_included
    #endinput
#endif
#define __vipm_core_objects_limits_type_included

#include <amxmodx>
#include <VipM/Limits>
#include <VipM/Params>
#include "VipM/Utils"
#include "VipM/ArrayMap"
#include "VipM/ArrayTrieUtils"
#include "VipM/Core/Objects/Param"

enum _:S_LimitType {
    LimitType_Name[VIPM_LIMITS_TYPE_NAME_MAX_LEN],
    Array:LimitType_Params, // S_CfgParam

    bool:LimitType_Static,
    LimitType_StaticValue,

    bool:LimitType_ForPlayer,

    LimitType_Events[E_LimitEvent],
}

enum T_LimitType { Invalid_LimitType = -1 }

static ArrayMap(g_amLimitTypes); // S_LimitType

LimitType_Init() {
    if (!ArrayMapCreated(g_amLimitTypes)) {
        InitArrayMap(g_amLimitTypes, S_LimitType);
    }
}

Array:LimitType_GetAll() {
    return ArrayClone(g_amLimitTypes[AM_Arr]);
}

LimitType_GetCount() {
    return ArraySizeSafe(g_amLimitTypes[AM_Arr]);
}

T_LimitType:LimitType_Construct(const sName[], const bool:bForPlayer = false, const bool:bStatic = false) {
    if (!ArrayMapCreated(g_amLimitTypes)) {
        abort(0, "Attempt to create limit type before limits controller init.");
        return Invalid_LimitType;
    }

    if (LimitType_Find(sName) != Invalid_LimitType) {
        abort(0, "Limit type '' already exists.", sName);
        return Invalid_LimitType;
    }

    new LimitType[S_LimitType];

    copy(LimitType[LimitType_Name], charsmax(LimitType[LimitType_Name]), sName);
    LimitType[LimitType_Params] = Invalid_Array;

    LimitType[LimitType_Static] = bStatic;
    LimitType[LimitType_StaticValue] = 0;

    LimitType[LimitType_ForPlayer] = bForPlayer;

    arrayset(LimitType[LimitType_Events], -1, sizeof(LimitType[LimitType_Events]));

    return T_LimitType:ArrayMapPushArray(g_amLimitTypes, LimitType, LimitType[LimitType_Name]);
}

T_LimitType:LimitType_Find(const sName[]) {
    return T_LimitType:ArrayMapGetIndex(g_amLimitTypes, sName);
}

static LimitType__Get(const T_LimitType:iLimitType, LimitType[S_LimitType]) {
    ArrayMapGetiArray(g_amLimitTypes, _:iLimitType, LimitType);
}

static LimitType__Set(const LimitType[S_LimitType]) {
    ArrayMapSetArray(g_amLimitTypes, LimitType[LimitType_Name], LimitType);
}

Array:LimitType_GetParams(const T_LimitType:iLimitType) {
    new LimitType[S_LimitType];
    LimitType__Get(iLimitType, LimitType);

    if (LimitType[LimitType_Params] == Invalid_Array) {
        return Invalid_Array;
    }

    return ArrayClone(LimitType[LimitType_Params]);
}

LimitType_MergeParams(const T_LimitType:iLimitType, const Array:aParams) {
    new LimitType[S_LimitType];
    LimitType__Get(iLimitType, LimitType);

    LimitType[LimitType_Params] = CfgParam_Merge(LimitType[LimitType_Params], aParams);

    LimitType__Set(LimitType);
}

Trie:LimitType_ReadParams(const T_LimitType:iLimitType, const JSON:jParams) {
    new LimitType[S_LimitType];
    LimitType__Get(iLimitType, LimitType);

    new Trie:tParams = TrieCreate();

    // У статических лимитов не может быть параметров
    if (LimitType[LimitType_Static]) {
        return tParams;
    }

    new sErrParam[VIPM_PARAM_NAME_MAX_LEN];
    if (!CfgParam_ReadList(jParams, tParams, LimitType[LimitType_Params], sErrParam, charsmax(sErrParam))) {
        Json_ErrorForFile(jParams, "Param '%s' required for '%s' limit.", sErrParam, LimitType[LimitType_Name]);
        TrieDestroy(tParams);
        return Invalid_Trie;
    }

    if (LimitType[LimitType_Events][Limit_OnRead] >= 0) {
        new iRet = VIPM_STOP;
        ExecuteForward(LimitType[LimitType_Events][Limit_OnRead], iRet, jParams, tParams);

        if (iRet == VIPM_STOP) {
            TrieDestroy(tParams);
            return Invalid_Trie;
        }
    }
    
    return tParams;
}

LimitType_SetEventListener(
    const T_LimitType:iLimitType,
    const E_LimitEvent:iEvent,
    const PluginId,
    const sFuncName[]
) {
    new LimitType[S_LimitType];
    LimitType__Get(iLimitType, LimitType);

    if (LimitType[LimitType_Events][iEvent] >= 0) {
        DestroyForward(LimitType[LimitType_Events][iEvent]);
    }
    
    LimitType[LimitType_Events][iEvent] = LimitType__MakeEventForward(iEvent, PluginId, sFuncName);

    if (LimitType[LimitType_Events][iEvent] < 0) {
        abort(0, "[ERROR] Can't create forward for func '%s' in plugin #%d.", sFuncName, PluginId);
    }

    LimitType__Set(LimitType);
}

static LimitType__MakeEventForward(const E_LimitEvent:iEvent, const PluginId, const sFuncName[]) {
    switch (iEvent) {
        case Limit_OnRead: // (const JSON:jCfg, Trie:tParams)
            return CreateOneForward(PluginId, sFuncName, FP_CELL, FP_CELL);
        case Limit_OnCheck: // (const Trie:tParams, const UserId)
            return CreateOneForward(PluginId, sFuncName, FP_CELL, FP_CELL);
    }

    abort(0, "[ERROR] Invalid event index (%d).", iEvent);
    return -1;
}

LimitType_SetStaticValue(const T_LimitType:iLimitType, const bool:bValue, const UserId = 0) {
    new LimitType[S_LimitType];
    LimitType__Get(iLimitType, LimitType);
    
    if (!LimitType[LimitType_Static]) {
        abort(0, "Trying to set static value for a non-static limit.");
        return;
    }

    if (UserId > 0) {
        BitSetIf(LimitType[LimitType_StaticValue], UserId - 1, bValue);
    } else {
        LimitType[LimitType_StaticValue] = bValue;
    }

    LimitType__Set(LimitType);
}

bool:LimitType_Execute(const T_LimitType:iLimitType, const Trie:tParams, const UserId = 0) {
    new LimitType[S_LimitType];
    LimitType__Get(iLimitType, LimitType);
    
    new bool:bRes = false;
    if (LimitType[LimitType_Static]) {
        if (LimitType[LimitType_ForPlayer]) {
            bRes = BitIs(LimitType[LimitType_StaticValue], UserId - 1);
        } else {
            bRes = !!LimitType[LimitType_StaticValue];
        }
    } else {
        if (LimitType[LimitType_Events][Limit_OnCheck] < 0) {
            abort(0, "'Limit_OnCheck' event listener for non-static limit type '%s' must be implemented.", LimitType[LimitType_Name]);
            bRes = false;
        } else {
            ExecuteForward(LimitType[LimitType_Events][Limit_OnCheck], bRes, tParams, UserId);
        }
    }

    return bRes;
}
