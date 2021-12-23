#include amxmodx

Natives_Limits_Init(){
    RegisterNative("Limits_RegisterType");
    RegisterNative("Limits_AddTypeParams");
    RegisterNative("Limits_RegisterTypeEvent");

    RegisterNative("Limits_ReadFromJson");
    RegisterNative("Limits_ReadListFromJson");

    RegisterNative("Limits_SetStaticValue");

    RegisterNative("Limits_Execute");
    RegisterNative("Limits_ExecuteList");
}

// native VipM_Limits_RegisterType(const sName[], const bool:bForPlayer = true, const bool:bStatic = false);
@_Limits_RegisterType(const PluginId){
    enum {Arg_LimitName = 1, Arg_ForPlayer, Arg_Static}

    new sName[32];
    get_string(Arg_LimitName, sName, charsmax(sName));

    new bool:bForPlayer = bool:get_param(Arg_ForPlayer);
    new bool:bStatic = bool:get_param(Arg_Static);
    
    Limits_AddType(sName, bForPlayer, bStatic, PluginId);
}

// native VipM_Limits_AddTypeParams(const sName[], any:...);
@_Limits_AddTypeParams(const PluginId, const cParams){
    enum {Arg_LimitName = 1, Arg_Params}

    new sName[32];
    get_string(Arg_LimitName, sName, charsmax(sName));
    
    NATIVE_CHECK_LIMIT_TYPE(sName)
    
    new LimitType[S_Limit];
    LIMIT_TYPE_GET(sName, LimitType);
    LimitType[Limit_Params] = Native_GetParamsList(Arg_Params - 1, cParams, LimitType[Limit_Params]);
    SET_LIMIT_TYPE(LimitType);
}

// native VipM_Limits_RegisterTypeEvent(const sName[], const E_LimitEvent:iEvent, const sFunc[]);
@_Limits_RegisterTypeEvent(const PluginId){
    enum {Arg_LimitName = 1, Arg_Event, Arg_FuncName}

    new sName[32], E_LimitEvent:iEvent, sFunc[64];
    get_string(Arg_LimitName, sName, charsmax(sName));
    iEvent = E_LimitEvent:get_param(Arg_Event);
    get_string(Arg_FuncName, sFunc, charsmax(sFunc));
    
    NATIVE_CHECK_LIMIT_TYPE(sName)
    
    Limits_RegisterEvent(PluginId, sName, iEvent, sFunc);
}

// native T_LimitUnit:VipM_Limits_ReadFromJson(const JSON:jLimit);
T_LimitUnit:@_Limits_ReadFromJson(){
    enum {Arg_JsonValue = 1}

    new JSON:jLimit = JSON:get_param(Arg_JsonValue);
    
    return Limits_LoadUnitFromJson(jLimit);
}

// native Array:VipM_Limits_ReadListFromJson(const JSON:jLimits, Array:aLimits = Invalid_Array);
Array:@_Limits_ReadListFromJson(){
    enum {Arg_JsonValue = 1, Arg_Array}

    new JSON:jLimits = JSON:get_param(Arg_JsonValue);
    new Array:aLimits = Array:get_param(Arg_Array);
    
    return Limits_LoadUnitListFromJson(jLimits, aLimits);
}

// native VipM_Limits_Execute(const sName[], const bool:bNewValue, const UserId = 0);
@_Limits_SetStaticValue(){
    enum {Arg_LimitName = 1, Arg_NewValue, Arg_UserId}

    static sName[32];
    get_string(Arg_LimitName, sName, charsmax(sName));
    new bool:bNewValue = bool:get_param(Arg_NewValue);
    new UserId = get_param(Arg_UserId);
    
    NATIVE_CHECK_LIMIT_TYPE(sName)
    
    Limits_SetStaticValue(sName, bNewValue, UserId);
}

// native bool:VipM_Limits_Execute(const T_LimitUnit:iLimit, const UserId = 0);
bool:@_Limits_Execute(){
    enum {Arg_LimitUnit = 1, Arg_UserId}

    new T_LimitUnit:iLimit = T_LimitUnit:get_param(Arg_LimitUnit);
    new UserId = get_param(Arg_UserId);
    
    return Limits_ExecuteUnit(iLimit, UserId);
}

// native bool:VipM_Limits_ExecuteList(const Array:aLimits, const UserId = 0, const E_LimitsExecType:iType = Limit_Exec_OR);
bool:@_Limits_ExecuteList(){
    enum {Arg_LimitUnitList = 1, Arg_UserId, Arg_Type}

    new Array:aLimits = Array:get_param(Arg_LimitUnitList);
    new UserId = get_param(Arg_UserId);
    new E_LimitsExecType:Type = E_LimitsExecType:get_param(Arg_Type);
    
    return Limits_ExecuteUnitList(aLimits, UserId, Type);
}
