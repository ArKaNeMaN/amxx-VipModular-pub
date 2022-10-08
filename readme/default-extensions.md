# Стандартные расширения

## Модули

- [SpawnHealth](extensions/modules/spawn-health.md) - Выдача очков здоровья и брони при спавне
- [SpawnItems](extensions/modules/spawn-items.md) - Выдача предметов при спавне
- [WeaponMenu](extensions/modules/weapon-menu.md) - Оружейное меню
- [VipInTab](extensions/modules/vip-in-tab.md) - Надпись VIP в таблице счёта игроков
- Vampire - Вампиризм

## Ограничения

- ForAll/Always - Условие всегда верно
- Never - Условие всегда ложно
- Steam - Условие верно для Steam-игроков
- Alive - Условие верно для живых игроков
- Bot - Условие верно для ботов
- Name - Условие верно для игроков с указанным ником
- Flags - Условие верно для игрков, имеющих указанные флаги
- SteamId - Усовие верно для игроков с указанным SteamID
- Ip - Условие верно для игроков с указанным IP-адресом
- Map - Условие верно на указанной карте
- HasPrimaryWeapon - Условие верно для игроков (не)имеющих основное оружие
- Round - Условие верно в указанные раунды
- WeekDay - Условие верно в указанный день недели
- GCMS-Service - Условие верно для игроков, имеющих указанную услугу
- GCMS-Member - Условие верно для игроков, зарегистрированных на сайте сервера
- ToD-DayTime - Условие верно в указанное время суток из [Time Of Day](https://arkanaplugins.ru/plugin/11)
- Logic-OR - Условие верно, когда верно хотя бы одно из указанных условий
- Logic-XOR - Условие верно, когда верно только одно из указанных условий
- Logic-AND - Условие верно, когда верно все указанные условия
- Logic-NOT - Условие верно, когда указанное условие не верное

## Типы предметов

- [Weapon](extensions/items/weapon.md) - Стандартное оружие
- [ItemsList](extensions/items/items-list.md) - Несколько предметов
- [Command](extensions/items/command.md) - Клиентская/серверная команда
- [DefuseKit](extensions/items/defuse-kit.md) - Набор сапёра
- [Cwapi](extensions/items/cwapi.md) - Кастомное оружие из [Custom Weapons API](https://github.com/ArKaNeMaN/amxx-CustomWeaponsAPI)
