#if defined _vipmodular_included
	#endinput
#endif
#define _vipmodular_included

#include amxmodx
#include json

stock const VIPM_LIBRARY[] = "VipModular";
#define _VIPM_VERSION "5.0.0-b12"
stock const VIPM_VERSION[] = _VIPM_VERSION;
stock const VIPM_CFG_PATH[] = "plugins/VipModular";
#define _VIPM_PLUGIN_URL "https://github.com/ArKaNeMaN/amxx-VipModular-pub"

/**
 * Получение пути, относительно папки с конфигами вип-системы или AmxModX.
 *
 * @note    Если название файла начинается со слеша `/`, файл будет взят из папки amxmodx/configs.
 *          Иначе из папки вип-системы (см. константу VIPM_CFG_PATH).
 *
 * @param Path      Путь / Название файла.
 * @param sOut      Буффер для записи полученного пути.
 * @param iOutLen   Размер буфера.
 *
 * @return  Кол-во записанных в буфер ячеек.
 */
stock VipM_GetCfgPath(const sPath[] = "", sOut[], const iOutLen){
    static __amxx_configsdir[PLATFORM_MAX_PATH];
    if (!__amxx_configsdir[0]) {
        get_localinfo("amxx_configsdir", __amxx_configsdir, charsmax(__amxx_configsdir));
    }

    if (!sPath[0]) {
        return formatex(sOut, iOutLen, "%s/%s", __amxx_configsdir, VIPM_CFG_PATH);
    }
    
    return (sPath[0] == '/')
        ? formatex(sOut, iOutLen, "%s%s", __amxx_configsdir, sPath)
        : formatex(sOut, iOutLen, "%s/%s/%s", __amxx_configsdir, VIPM_CFG_PATH, sPath);
}

/**
 * Получение пути, относительно папки с конфигами вип-системы или AmxModX.
 *
 * @note    Если название файла начинается со слеша `/`, файл будет взят из папки amxmodx/configs.
 *          Иначе из папки вип-системы (см. константу VIPM_CFG_PATH).
 *
 * @param Path      Путь / Название файла.
 *
 * @return  Полученный путь.
 */
stock VipM_iGetCfgPath(const sPath[] = "") {
    new sRetPath[PLATFORM_MAX_PATH];
    VipM_GetCfgPath(sPath, sRetPath, charsmax(sRetPath));
    return sRetPath;
}
// inl_VipM_GetCfgPath is deprecated
#define inl_VipM_GetCfgPath VipM_iGetCfgPath

enum _:VipM_FwdReturn{
    /*
     * Продолжить действие
     */
    VIPM_CONTINUE = 0,
    
    /*
     * Прервать действие
     */
    VIPM_STOP = 1,
}

/**
 * Вызывается в момент инициалзиации модулей и лимитов.
 *
 * @note    Вызывается из plugin_precache
 *
 * @noreturn
 */
forward VipM_OnInitModules();

/**
 * Вызывается в момент чтения элемента конфига (модуля или лимита).
 *
 * @param jUnit     JSON-обьект с параметрами
 * @param tParams   Хэш-карта с прочитанными параметрами. Может быть изменена.
 *
 * @note    Параметр tParams содержит только прочитанные ядром параметры.
 *
 * @note    Соответствует событиям Limit_OnRead и Module_OnRead,
 *          и форвардам VipM_OnReadLimitUnit и VipM_OnReadModuleUnit.
 *          Вызывается перед ними.
 *
 * @return По умолчанию VIPM_CONTINUE. Если вернуть VIPM_STOP, элемент конфига будет проигнорирован.
 */
forward VipM_OnReadUnit(const JSON:jUnit, const Trie:tParams);

/**
 * Вызывается после полной загрузки системы привилегий.
 *
 * @note    Вызывается из plugin_precache
 *
 * @noreturn
 */
forward VipM_OnLoaded();

/**
 * Вызывается после загрузки привилегий игрока.
 *
 * @param UserId    Индекс игрока, чьи привилегии были загружены.
 *
 * @note            Можно использовать для кэширования параметров привилегий.
 *
 * @noreturn
 */
forward VipM_OnUserUpdated(const UserId);

/**
 * Обновляет привилегии указанного пользователя.
 *
 * @param UserId    Индекс игрока, чьи привилегии надо обновить.
 *
 * @noreturn
 */
native VipM_UserUpdate(const UserId);

/**
 * Выводит лог с указанием JSON-файла, из которого было прочитано указанное значене.
 *
 * @param jValue    Значение, для которого надо вывести лог.
 * @param sPrefix   Префикс лога. Например: ERROR, WARNING, INFO
 * @param sMessage  Текст сообщения.
 * @param any:...   Параметры форматирования сообщения.
 *
 * @noreturn
 */
native VipM_Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...);

#include <VipM/Modules>
#include <VipM/Limits>
#include <VipM/ItemsController>
#include <VipM/Params>
#include <VipM/M/WeaponMenu>
