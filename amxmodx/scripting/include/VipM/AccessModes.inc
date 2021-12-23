#if defined _vipmodular_am_included
	#endinput
#endif
#define _vipmodular_am_included

/**
 * Vip Modular: Access Modes
*/

enum E_AccessModeEvent{

    /*
     * Описание:    Вызывается, когда читаются параметры режима доступа.
     * Возв. тип:   VipM_FwdReturn
     * Параметры:   (const JSON:jCfg, Trie:Params):
     *      jCfg - JSON-обьект с параметрами.
     *      Params - Хэш-карта с прочитанными параметрами. Может быть изменена.
     * 
     * Примечание: Вызывается в plugin_precache
     * Примечание: Если возвращено VIPM_STOP, элемент дотсупа будет пропущен.
     */
    AccessMode_OnRead,

    /*
     * Описание:    Вызывается, когда проверяется наличие у игрока доступа данным режимом.
     * Возв. тип:   bool
     * Параметры:   (const UserId, const Trie:Params):
     *      UserId - Индекс игрока, чей доступ проверяется.
     *      Params - Хэш-карта с параметрами. Для извлечения отдельных значений можно использовать стоковые функции VipM_Params_*.
     * 
     * Примечание: Если возвращено true, доступ получен.
     */
    AccessMode_OnCheck,
}

/**
 * Регистрирует новый режим доступа.
 *
 * @param AccessMode    Название режима доступа.
 *
 * @noreturn
 */
native VipM_RegisterAccessMode(const AccessMode[]);

/**
 * Регистрирует обработчик события для указанного режима доступа.
 *
 * @param AccessMode    Название режима доступа.
 * @param Event         Событие.
 * @param Func          Название функции-обработчика.
 *
 * @return  Вернёт true, если обработчик успешно зарегистрирован.
 */
native VipM_RegisterAccessModeEvent(const AccessMode[], const E_AccessModeEvent:Event, const Func[]);

/**
 * Добавляет набор параметров для указанного режима доступа.
 *
 * @param AccessMode    Название режима доступа.
 * @param any:...       Перечисление параметров.
 *
 * @note    Формат указания параметров: VipM_AddAccessModeParams(..., const ParamName[], const E_ParamType:ParamType, const bool:ParamRequired, ...);
 * @note    ParamName - Название параметра / ParamType - Тип параметра / ParamRequired - Обязателен ли параметр.
 * @note    Пример: VipM_AddAccessModeParams("ExampleAccessMode", "Param1", ptInteger, true, "Param2", ptCustom, false);
 *
 * @noreturn
 */
native VipM_AddAccessModeParams(const AccessMode[], const any:...);
#define VipM_SetAccessModeParams VipM_AddAccessModeParams
