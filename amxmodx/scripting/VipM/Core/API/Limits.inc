#include <amxmodx>
#include <VipM/Limits>
#include "VipM/Natives"

#include "VipM/Core/Objects/Limits/Type"
#include "VipM/Core/Objects/Limits/Unit"
#include "VipM/Core/Objects/Param"

API_Limits_Init() {
    Natives_Init("VipM");

    Natives_Reg("Limits_RegisterType");
    Natives_Reg("Limits_AddTypeParams");
    Natives_Reg("Limits_RegisterTypeEvent");

    Natives_Reg("Limits_ReadFromJson");
    Natives_Reg("Limits_ReadListFromJson");

    Natives_Reg("Limits_SetStaticValue");

    Natives_Reg("Limits_Execute");
    Natives_Reg("Limits_ExecuteList");
}

// native VipM_Limits_RegisterType(const sLimitTypeName[], const bool:bForPlayer = true, const bool:bStatic = false);
T_LimitType:@_Limits_RegisterType() {
    enum {Arg_sLimitTypeName = 1, Arg_bForPlayer, Arg_bStatic}

    new sLimitTypeName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    get_string(Arg_sLimitTypeName, sLimitTypeName, charsmax(sLimitTypeName));

    new bool:bForPlayer = bool:get_param(Arg_bForPlayer);
    new bool:bStatic = bool:get_param(Arg_bStatic);
    
    new T_LimitType:iLimitType = LimitType_Construct(sLimitTypeName, bForPlayer, bStatic);
    if (iLimitType == Invalid_LimitType) {
        log_error(0, "Can't create limit type '%s'.", sLimitTypeName);
        return Invalid_LimitType;
    }

    return iLimitType;
}

// native VipM_Limits_AddTypeParams(const sLimitTypeName[], any:...);
@_Limits_AddTypeParams(const PluginId, const iParamsNum) {
    enum {Arg_sLimitTypeName = 1, Arg_Params}

    new sLimitTypeName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    get_string(Arg_sLimitTypeName, sLimitTypeName, charsmax(sLimitTypeName));

    new T_LimitType:iLimitType = LimitType_Find(sLimitTypeName);
    if (iLimitType == Invalid_LimitType) {
        log_error(0, "Limit type '%s' not found.", sLimitTypeName);
        return;
    }

    new Array:aParams = CfgParam_GetFromNative(Arg_Params, iParamsNum);
    if (aParams == Invalid_Array) {
        return;
    }

    LimitType_MergeParamsList(iLimitType, aParams);
}

// native VipM_Limits_RegisterTypeEvent(const sLimitTypeName[], const E_LimitEvent:iEvent, const sFunc[]);
@_Limits_RegisterTypeEvent(const PluginId) {
    enum {Arg_sLimitTypeName = 1, Arg_iEvent, Arg_sFuncName}

    new sLimitTypeName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    get_string(Arg_sLimitTypeName, sLimitTypeName, charsmax(sLimitTypeName));

    new T_LimitType:iLimitType = LimitType_Find(sLimitTypeName);
    if (iLimitType == Invalid_LimitType) {
        log_error(0, "Limit type '%s' not found.", sLimitTypeName);
        return;
    }

    new E_LimitEvent:iEvent = E_LimitEvent:get_param(Arg_iEvent);

    new sFuncName[64];
    get_string(Arg_sFuncName, sFuncName, charsmax(sFuncName));
    
    LimitType_SetEventListener(iLimitType, iEvent, PluginId, sFuncName);
}

// native VipM_Limits_Execute(const sLimitTypeName[], const bool:bNewValue, const UserId = 0);
@_Limits_SetStaticValue() {
    enum {Arg_sLimitTypeName = 1, Arg_NewValue, Arg_UserId}

    static sLimitTypeName[VIPM_LIMITS_TYPE_NAME_MAX_LEN];
    get_string(Arg_sLimitTypeName, sLimitTypeName, charsmax(sLimitTypeName));

    new T_LimitType:iLimitType = LimitType_Find(sLimitTypeName);
    if (iLimitType == Invalid_LimitType) {
        log_error(0, "Limit type '%s' not found.", sLimitTypeName);
        return;
    }

    new bool:bNewValue = bool:get_param(Arg_NewValue);
    new UserId = get_param(Arg_UserId);
    
    LimitType_SetStaticValue(iLimitType, bNewValue, UserId);
}

// native T_LimitUnit:VipM_Limits_ReadFromJson(const JSON:jLimit);
T_LimitUnit:@_Limits_ReadFromJson() {
    enum {Arg_JsonValue = 1}

    new JSON:jLimit = JSON:get_param(Arg_JsonValue);
    
    return LimitUnit_Read(jLimit);
}

// native Array:VipM_Limits_ReadListFromJson(const JSON:jLimits, Array:aLimits = Invalid_Array);
Array:@_Limits_ReadListFromJson() {
    enum {Arg_JsonValue = 1, Arg_Array}

    new JSON:jLimits = JSON:get_param(Arg_JsonValue);
    new Array:aLimits = Array:get_param(Arg_Array);
    
    return LimitUnit_ReadList(jLimits, aLimits);
}

// native bool:VipM_Limits_Execute(const T_LimitUnit:iLimit, const UserId = 0);
bool:@_Limits_Execute() {
    enum {Arg_LimitUnit = 1, Arg_UserId}

    new T_LimitUnit:iLimit = T_LimitUnit:get_param(Arg_LimitUnit);
    new UserId = get_param(Arg_UserId);
    
    return LimitUnit_Execute(iLimit, UserId);
}

// native bool:VipM_Limits_ExecuteList(const Array:aLimits, const UserId = 0, const E_LimitsExecType:iType = Limit_Exec_OR);
bool:@_Limits_ExecuteList() {
    enum {Arg_LimitUnitList = 1, Arg_UserId, Arg_Type}

    new Array:aLimits = Array:get_param(Arg_LimitUnitList);
    new UserId = get_param(Arg_UserId);
    new E_LimitsExecType:Type = E_LimitsExecType:get_param(Arg_Type);
    
    return LimitUnit_ExecuteList(aLimits, UserId, Type);
}
