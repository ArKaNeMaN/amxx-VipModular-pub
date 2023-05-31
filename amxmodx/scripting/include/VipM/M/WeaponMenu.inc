#if defined _vipmodular_m_weaponmenu_included
	#endinput
#endif
#define _vipmodular_m_weaponmenu_included

#define VIPM_M_WEAPONMENU_EXPIRE_STATUS_MAX_LEN 64

/**
 * Устанавливает статус окончания привилегий, выводимый игроку в оружейном меню.
 * 
 * @param UserId        Индекс игрока, которому будет установлен статус.
 * @param sNewStatus    Устанавливаемый статус.
 * 
 * @note    При установке пустого статуса, соответствующий пункт в меню будет скрыт.
 * @note    По умолчанию статус пустой.
 * 
 * @noreturn
 */
native VipM_WeaponMenu_SetExpireStatus(const UserId, const sNewStatus[]);