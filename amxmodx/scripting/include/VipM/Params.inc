#if defined _vipmodular_params_included
	#endinput
#endif
#define _vipmodular_params_included

#include amxmodx

/**
 * Vip Modular: Parameters
*/

enum E_ParamType {

    /*
     * Пользовательский тип параметра. Подразумевает использование события OnRead.
     * Обязательность таких параметров игнорируется.
     */
    ptCustom = 0,
    
    /*
     * Целое число.
     */
    ptInteger,
    
    /*
     * Дробное число.
     */
    ptFloat,
    
    /*
     * true/false.
     */
    ptBoolean,
    
    /*
     * Строка.
     */
    ptString,
    
    /*
     * Цвет (Массив из трёх целых чисел).
     */
    ptColor,
    
    /*
     * Двумерный вектор (Массив из двух дробных чисел).
     */
    ptVector2,
    
    /*
     * Трёхмерный вектор (Массив из трёх дробных чисел).
     */
    ptVector3,
    
    /*
     * Условный элемент.
     */
    ptLimit,
    
    /*
     * Массив условных элементов.
     */
    ptLimits,
}
stock const VIPM_PARAM_TYPE_NAMES[E_ParamType][] = {"Custom", "Integer", "Float", "Bool", "String", "Color", "Vector2", "Vector3", "Limit", "Limits"};

stock any:VipM_Params_GetCell(const Trie:tParams, const sKey[], const any:iDef = 0) {
    if (tParams == Invalid_Trie) {
        return iDef;
    }

    new any:iVal;
    return TrieGetCell(tParams, sKey, iVal) ? iVal : iDef;
}

stock VipM_Params_GetInt(const Trie:tParams, const sKey[], const iDef = 0) {
    return _:VipM_Params_GetCell(tParams, sKey, iDef);
}

stock bool:VipM_Params_GetBool(const Trie:tParams, const sKey[], const bool:bDef = false) {
    return VipM_Params_GetCell(tParams, sKey, bDef) ? true : false;
}

stock Float:VipM_Params_GetFloat(const Trie:tParams, const sKey[], const Float:fDef = 0.0) {
    return Float:VipM_Params_GetCell(tParams, sKey, fDef);
}

stock VipM_Params_GetStr(const Trie:tParams, const sKey[], sOut[], const iOutLen, const sDef[] = NULL_STRING) {
    new iWrittenLen = 0;
    if (
        tParams != Invalid_Trie
        && TrieGetString(tParams, sKey, sOut, iOutLen, iWrittenLen)
    ) {
        return iWrittenLen;
    }

    return copy(sOut, iOutLen, sDef);
}

stock VipM_Params_GetStri(const Trie:tParams, const sKey[], const sDef[] = NULL_STRING) {
    new sStr[MAX_FMT_LENGTH]
    if (
        tParams == Invalid_Trie
        || !TrieGetString(tParams, sKey, sStr, charsmax(sStr))
    ) {
        copy(sStr, charsmax(sStr), sDef);
    }

    return sStr;
}

stock Array:VipM_Params_GetArr(const Trie:tParams, const sKey[]) {
    return Array:VipM_Params_GetCell(tParams, sKey, Invalid_Array);
}

stock T_LimitUnit:VipM_Params_GetLimit(const Trie:tParams, const sKey[], const T_LimitUnit:iDef = Invalid_LimitUnit) {
    return T_LimitUnit:VipM_Params_GetCell(tParams, sKey, iDef);
}

stock bool:VipM_Params_ExecuteLimit(
    const Trie:tParams,
    const sKey[] = "Limit",
    const UserId = 0,
    const bool:bDef = true
) {
    new T_LimitUnit:iLimit = VipM_Params_GetLimit(tParams, sKey);
    if (iLimit == Invalid_LimitUnit) {
        return bDef;
    }

    return VipM_Limits_Execute(iLimit, UserId);
}

stock bool:VipM_Params_ExecuteLimitsList(
    const Trie:tParams,
    const sKey[] = "Limits",
    const UserId = 0,
    const E_LimitsExecType:iType = Limit_Exec_OR,
    const bool:bDef = true
) {
    new Array:aLimits = VipM_Params_GetArr(tParams, sKey);
    if (aLimits == Invalid_Array) {
        return bDef;
    }
    
    return VipM_Limits_ExecuteList(aLimits, UserId, iType);
}
