#if defined __vipm_core_objects_limits_unit_included
    #endinput
#endif
// Два кванта
#define __vipm_core_objects_limits_unit_included

#include <amxmodx>
#include <VipM/Limits>
#include "VipM/Utils"
#include "VipM/ArrayMap"
#include "VipM/ArrayTrieUtils"

#include "VipM/Core/Objects/Limits/Type"

// T_LimitUnit

enum _:S_LimitUnit {
    T_LimitType:LimitUnit_Type,
    Trie:LimitUnit_Params,
}

static Array:g_aLimitUnits = Invalid_Array;

LimitUnit_Init() {
    CallOnce();

    LimitType_Init();
    
    g_aLimitUnits = ArrayCreate(S_LimitUnit, 1);
    
    Forwards_RegAndCall("VipM_Limits_OnInited", ET_IGNORE);
}

LimitUnit_GetCount() {
    return ArraySizeSafe(g_aLimitUnits);
}

static T_LimitUnit:LimitUnit__Construct(const T_LimitType:iLimitType, const Trie:tParams) {
    if (g_aLimitUnits == Invalid_Array) {
        abort(AMX_ERR_GENERAL, "Attempt to create limit unit before init.");
        return Invalid_LimitUnit;
    }
    
    new LimitUnit[S_LimitUnit];

    LimitUnit[LimitUnit_Type] = iLimitType;
    LimitUnit[LimitUnit_Params] = tParams;

    return T_LimitUnit:ArrayPushArray(g_aLimitUnits, LimitUnit);
}

static bool:LimitUnit__Get(const T_LimitUnit:iLimitUnit, LimitUnit[S_LimitUnit], const bool:bOrFail = true) {
    if (_:iLimitUnit < 0 || _:iLimitUnit >= LimitUnit_GetCount()) {
        if (bOrFail) {
           abort(AMX_ERR_NOTFOUND, "Invalid limit unit index (%d).", iLimitUnit);
        }
        
        return false;
    }

    ArrayGetArray(g_aLimitUnits, _:iLimitUnit, LimitUnit);
    return true;
}

static T_LimitType:LimitUnit__ReadType(const JSON:jLimitUnit) {
    new sLimitTypeName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    Json_Object_GetString(jLimitUnit, "Limit", sLimitTypeName, charsmax(sLimitTypeName));
    trim(sLimitTypeName);

    if (!sLimitTypeName[0]) {
        PCJson_ErrorForFile(jLimitUnit, "Limit type name not specified.");
        return Invalid_LimitType;
    }

    new T_LimitType:iLimitType = LimitType_Find(sLimitTypeName);

    if (iLimitType == Invalid_LimitType) {
        PCJson_ErrorForFile(jLimitUnit, "Limit type '%s' not found.", sLimitTypeName);
        return Invalid_LimitType;
    }

    return iLimitType;
}

T_LimitUnit:LimitUnit_Read(const JSON:valueJson) {
    static Trie:cache = Invalid_Trie;
    TrieCreateIfNotCreated(cache);
    
    new JSON:linked, T_LimitUnit:cached = Invalid_LimitUnit, linkPath[PLATFORM_MAX_PATH];
    if (PCJson_HandleCachedLinkedValue(cache, valueJson, linked, cached, linkPath, charsmax(linkPath)) == PCJson_CachedLinkedValue_Cached) {
        return cached;
    }

    new T_LimitType:type = LimitUnit__ReadType(linked);
    if (type == Invalid_LimitType) {
        PCJson_FreeLinked(linked);
        return Invalid_LimitUnit;
    }

    new Trie:p = LimitType_ReadParams(type, linked);
    if (p == Invalid_Trie) {
        PCJson_FreeLinked(linked);
        return Invalid_LimitUnit;
    }

    cached = LimitUnit__Construct(type, p);
    if (linkPath[0] != EOS && cached != Invalid_LimitUnit) {
        TrieSetCell(cache, linkPath, cached);
    }

    PCJson_FreeLinked(linked);
    return cached;
}

Array:LimitUnit_ReadList(const JSON:jLimitUnits, &Array:aLimitUnits = Invalid_Array) {
    ArrayCreateIfNotCreated(aLimitUnits, 1, 1);

    if (!json_is_array(jLimitUnits)) {
        new T_LimitUnit:iLimitUnit = LimitUnit_Read(jLimitUnits);
        if (iLimitUnit != Invalid_LimitUnit) {
            ArrayPushCell(aLimitUnits, iLimitUnit);    
        }
    } else {
        json_array_foreach_value (jLimitUnits: i => jLimitUnit) {
            aLimitUnits = LimitUnit_ReadList(jLimitUnit, aLimitUnits);
            json_free(jLimitUnit);
        }
    }

    return aLimitUnits;
}

Array:JsonObject_GetLimitUnits(const JSON:jObj, const sKey[], const bool:bDotNot = false, &Array:aLimitUnits = Invalid_Array) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return aLimitUnits;
    }

    new JSON:jLimitUnits = json_object_get_value(jObj, sKey, bDotNot);
    aLimitUnits = LimitUnit_ReadList(jLimitUnits, aLimitUnits);
    json_free(jLimitUnits);

    return aLimitUnits;
}

bool:LimitUnit_Execute(const T_LimitUnit:iLimitUnit, const UserId = 0) {
    new LimitUnit[S_LimitUnit];
    LimitUnit__Get(iLimitUnit, LimitUnit);

    return LimitType_Execute(LimitUnit[LimitUnit_Type], LimitUnit[LimitUnit_Params], UserId);
}

bool:LimitUnit_ExecuteList(
    const Array:aLimitUnits,
    const UserId = 0,
    const E_LimitsExecType:iOperator
) {
    new bool:bXor = false;

    for (new i = 0, ii = ArraySizeSafe(aLimitUnits); i < ii; ++i) {
        new T_LimitUnit:iLimitUnit = T_LimitUnit:ArrayGetCell(aLimitUnits, i);
        new bool:bRes = LimitUnit_Execute(iLimitUnit, UserId);

        switch (iOperator) {
            case Limit_Exec_OR: {
                if (bRes) {
                    return true;
                }
            }
            case Limit_Exec_AND: {
                if (!bRes) {
                    return false;
                }
            }
            case Limit_Exec_XOR: {
                if (bRes && bXor) {
                    return false;
                }
                bXor = bXor || bRes;
            }
        }
    }
    
    switch (iOperator) {
        case Limit_Exec_OR: {
            return false;
        }
        case Limit_Exec_AND: {
            return true;
        }
        case Limit_Exec_XOR: {
            return bXor;
        }
    }

    return false;
}
