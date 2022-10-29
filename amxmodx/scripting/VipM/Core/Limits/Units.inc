#include <amxmodx>
#include <json>

// Structs

enum _:S_LimitUnit{
    T_Limit:LimitUnit_LimitId,
    Trie:LimitUnit_Params, // S_CfgParam
}
new Array:LimitUnits;

// Array utils

#define Limits_PushUnit(%1) \
    T_LimitUnit:(ArrayPushArray(LimitUnits, %1))

#define Limits_GetUnit(%1,%2) \
    ArrayGetArray(LimitUnits, _:%1, %2)

#define Limits_IsUnitValid(%1) \
    (_:%1 >= 0 && _:%1 < ArraySizeSafe(LimitUnits))

// Cache

new Trie:__cache_LimitUnits;

T_LimitUnit:Limits_AddUnitToCache(const T_LimitUnit:iLimit, const CacheKey[] = ""){
    if (CacheKey[0]) {
        TrieSetCell(__cache_LimitUnits, CacheKey, iLimit);
    }
    return iLimit;
}

T_LimitUnit:Limits_GetUnitFromCache(const CacheKey[]){
    new T_LimitUnit:iLimit;
    return TrieGetCell(__cache_LimitUnits, CacheKey, iLimit)
        ? iLimit
        : Invalid_LimitUnit;
}

#define Limits_CacheExists(%1) \
    TrieKeyExists(__cache_LimitUnits, %1)

// Reader

bool:Limits_ReadUnitFromJson(const JSON:jUnit, Unit[S_LimitUnit]){
    new LimitName[32];
    json_object_get_string(jUnit, "Limit", LimitName, charsmax(LimitName));
    json_object_remove(jUnit, "Limit");

    if (!LIMIT_TYPE_EXISTS(LimitName)) {
        log_amx("[WARNING] Limit type `%s` not found.", LimitName);
        return false;
    }

    new LimitType[S_Limit];
    LIMIT_TYPE_GET(LimitName, LimitType);
    
    Unit[LimitUnit_LimitId] = LIMIT_TYPE_GET_ID(LimitName);
    Unit[LimitUnit_Params] = TrieCreate();
    if (!LimitType[Limit_Static]) {
        new ErrParam[32];
        if (!Cfg_ReadParams(jUnit, Unit[LimitUnit_Params], LimitType[Limit_Params], ErrParam, charsmax(ErrParam))) {
            log_amx("[WARNING] Param `%s` required for limit `%s`, but not found.", ErrParam, LimitType[Limit_Name]);
            return false;
        }
    }

    Forwards_DefaultReturn(VIPM_CONTINUE);
    if (
        Forwards_CallP("ReadUnit", jUnit, Unit[LimitUnit_Params]) == VIPM_STOP
        || Forwards_CallP("ReadLimitUnit", jUnit, Unit[LimitUnit_Params]) == VIPM_STOP
    ) {
        TrieDestroySafe(Unit[LimitUnit_Params]);
        return false;
    }
    
    new ret;
    EMIT_LIMIT_TYPE_EVENT(LimitType, Limit_OnRead, ret, jUnit, Unit[LimitUnit_Params])

    if (ret == VIPM_STOP) {
        TrieDestroySafe(Unit[LimitUnit_Params]);
        return false;
    }

    return true;
}

// Loaders

T_LimitUnit:Limits_LoadUnitFromFile(const FileName[]){
    if (Limits_CacheExists(FileName)) {
        return Limits_GetUnitFromCache(FileName);
    }
    
    if (!JSON_FILE_EXTSTS(FileName)) {
        log_amx("[WARNING] File `%s` not found.", FileName);
        return Invalid_LimitUnit;
    }

    new JSON:jUnit = GET_FILE_JSON(FileName);
    new T_LimitUnit:iLimit = Limits_LoadUnitFromJson(jUnit);
    json_free(jUnit);

    return Limits_AddUnitToCache(iLimit, FileName);
}

T_LimitUnit:Limits_LoadUnitFromJson(const JSON:jUnit){
    if (jUnit == Invalid_JSON) {
        return Invalid_LimitUnit;
    }

    new Ref[128];
    if (Json_IsRef(jUnit, Ref, charsmax(Ref))) {
        return Limits_LoadUnitFromFile(Ref);
    }

    new Unit[S_LimitUnit];
    if (Limits_ReadUnitFromJson(jUnit, Unit)) {
        return Limits_PushUnit(Unit);
    }
    
    return Invalid_LimitUnit;
}

Array:Limits_LoadUnitListFromJson(const JSON:jLimits, Array:aLimits = Invalid_Array){
    new T_LimitUnit:iLimit;
    if (!json_is_array(jLimits)) {
        ArrayCreateIfNotCreated(aLimits, 1, 1);

        iLimit = Limits_LoadUnitFromJson(jLimits);
        if (iLimit != Invalid_LimitUnit) {
            ArrayPushCell(aLimits, iLimit);
        }
    } else {
        ArrayCreateIfNotCreated(aLimits, 1, json_array_get_count(jLimits));

        json_array_foreach_value(jLimits: i => jLimit){
            iLimit = Limits_LoadUnitFromJson(jLimit);
            if (iLimit != Invalid_LimitUnit) {
                ArrayPushCell(aLimits, iLimit);
            }
            json_free(jLimit);
        }
    }

    if (ArraySizeSafe(aLimits) < 1) {
        // log_amx("[WARNING] Limit units list not loaded (invalid or empty).");
        ArrayDestroy(aLimits);
    }

    return aLimits;
}

// Usage

bool:Limits_ExecuteUnit(const T_LimitUnit:iLimit, const UserId = 0){
    if (!Limits_IsUnitValid(iLimit)) {
        log_error(0, "[ERROR] Invalid limit unit index (%d).", iLimit);
        return true;
    }
        
    new Limit[S_LimitUnit];
    Limits_GetUnit(iLimit, Limit);

    return Limits_Execute(Limit[LimitUnit_LimitId], Limit[LimitUnit_Params], UserId);
}

bool:Limits_ExecuteUnitList(const Array:aLimits, const UserId = 0, const E_LimitsExecType:Type = Limit_Exec_OR){
    new cLimits = ArraySizeSafe(aLimits);
    if (!cLimits) {
        return true;
    }

    new __xor_counter = 0;
    for (new i = 0; i < cLimits; i++) {
        new T_LimitUnit:iLimit = T_LimitUnit:ArrayGetCell(aLimits, i);
        switch (Type) {
            case Limit_Exec_OR: {
                if (Limits_ExecuteUnit(iLimit, UserId)) {
                    return true;
                }
            }
            
            case Limit_Exec_AND: {
                if (!Limits_ExecuteUnit(iLimit, UserId)) {
                    return false;
                }
            }

            case Limit_Exec_XOR: {
                if (Limits_ExecuteUnit(iLimit, UserId)) {
                    __xor_counter++;
                }
                
                if (__xor_counter > 1) {
                    return false;
                }
            }
        }
    }
    
    switch(Type){
        case Limit_Exec_OR:
            return false;
        
        case Limit_Exec_AND:
            return true;

        case Limit_Exec_XOR:
            return bool:__xor_counter;
    }

    return false;
}
