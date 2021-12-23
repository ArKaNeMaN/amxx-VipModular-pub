#if defined _vipmodular_l_included
	#endinput
#endif
#define _vipmodular_l_included

/**
 * Vip Modular: Limits
*/

enum T_LimitUnit {Invalid_LimitUnit = -1}

enum E_LimitEvent{

    /*
     * Описание:    Вызывается после прочтения параметров условного элемента.
     * Возв. тип:   VipM_FwdReturn
     * Параметры:   (const JSON:jCfg, Trie:Params):
     *      jCfg - JSON-обьект с параметрами.
     *      Params - Хэш-карта с прочитанными параметрами. Может быть изменена.
     * 
     * Примечание: Вызывается в plugin_precache
     * Примечание: Если возвращено VIPM_STOP, элемент дотсупа будет пропущен.
     */
    Limit_OnRead,

    /*
     * Описание:    Вызывается при проверке выполнения условия.
     * Возв. тип:   bool
     * Параметры:   (const Trie:Params, const UserId):
     *      Params - Хэш-карта с параметрами. Для извлечения отдельных значений можно использовать стоковые функции VipM_Params_*.
     *      UserId - Индекс игрока. 0, если условный элемент не зависит от игрока.
     * 
     * Примечание: Если возвращено true, условие выполняется.
     * Примечание: Не вызывается для статических условных элементов.
     */
    Limit_OnCheck,
}

enum E_LimitsExecType {
    
    /* ИЛИ */
    Limit_Exec_OR,

    /* И */
    Limit_Exec_AND,

    /* Исключающее ИЛИ */
    Limit_Exec_XOR,
}

native VipM_Limits_RegisterType(const sName[], const bool:bForPlayer = true, const bool:bStatic = false);
native VipM_Limits_AddTypeParams(const sName[], any:...);
native VipM_Limits_RegisterTypeEvent(const sName[], const E_LimitEvent:iEvent, const sFunc[]);

native T_LimitUnit:VipM_Limits_ReadFromJson(const JSON:jLimit);
native Array:VipM_Limits_ReadListFromJson(const JSON:jLimits, Array:aLimits = Invalid_Array);

native VipM_Limits_SetStaticValue(const sName[], const bool:bNewValue, const UserId = 0);

native bool:VipM_Limits_Execute(const T_LimitUnit:iLimit, const UserId = 0);
native bool:VipM_Limits_ExecuteList(const Array:aLimits, const UserId = 0, const E_LimitsExecType:iType = Limit_Exec_OR);
