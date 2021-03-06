# Стандартные типы предметов

Описание типов предметов, поставляемых вместе с системой привилегий.

## Cwapi

Кастомное оружие из плагина [Custom Weapons API](https://github.com/ArKaNeMaN/amxx-CustomWeaponsAPI).

_Для запуска должен быть установлен Custom Weapons API._

### Пример

```jsonc
{
    "Type": "Cwapi",

    "Name": "Vip_Ak47",
    "GiveType": "Replace"
}
```

### Параметры

- `Name`
  - Название оружия из CWAPI.
- `GiveType`
  - Тип выдачи оружия.
  - Доступные типы:
    - `Smart` - Для ножей заменить, для гранат добавить, для остального выбросить     текущее и выдать новое
    - `Append` или `Add` - Добавить к текущему в соответствующем слоте
    - `Replace` - Заменить оружие из соответствующего слота на новое
    - `Drop` - Выбросить текущее в соответствующем слоте и выдать новое

## ItemsList

Список любых предметов.

### Пример

```jsonc
{
    "Type": "ItemsList",

    "Items": [
        {
            "Type": "Cwapi",

            "Name": "Vip_Ak47",
            "GiveType": "Replace"
        },
        {
            "Type": "Cwapi",

            "Name": "Vip_Deagle",
            "GiveType": "Replace"
        }
    ]
}
```

### Параметры

- `Items`
  - Массив обьектов предметов.

## Weapon

Стандартное оружие из игры с префиксом `weapon_`.

### Пример

```jsonc
{
    "Type": "Weapon",

    "Name": "weapon_ak47",
    "GiveType": "Replace",
    "BpAmmo": 30
}
```

### Параметры

- `Name`
  - Название оружия из CWAPI.
- `BpAmmo`
  - Количество запасных патронов.
- `GiveType`
  - Тип выдачи оружия.
  - Доступные типы:
    - `GT_APPEND` или `Append` или `Add` - Добавить к текущему в соответствующем слоте
    - `GT_REPLACE` или `Replace` - Заменить оружие из соответствующего слота на новое
    - `GT_DROP_AND_REPLACE` или `Drop или`DropAndReplace` - Выбросить текущее в соответствующем слоте и выдать новое

## Command

Список любых предметов.

### Пример

```jsonc
{
    "Type": "Command",

    "Command": "HealthNade_Give {UserId}",
    "ByServer": true
}
```

### Параметры

- `Command`
  - Серверная или клиентская команда.
  - Возможна подстановка значений. Доступные значения:
    - `{UserId}` - индекс игрока, которому выдаётся этот предмет.
- `ByServer`
  - Если равно `true`, команда будет выполнена как серверная, иначе как клиентская.

## DefuseKit

Набор сапёра (Только для команды CT, для TT будет игнорироваться).

### Пример

```jsonc
{
    "Type": "DefuseKit"
}
```

### Параметры

_Параметры отсутствуют._
