# SpawnItems

Выдача предметов при спавне.

_Данный модуль использует контроллер предметов (`VipM-ItemsController`), поставляемый в комплекте с системой привилегий._

## Пример

```json
{
    "Module": "SpawnItems",

    "Limits": {
        "Limit": "Round",
        "Min": 2
    },

    "Items": [
        {
            "Type": "Weapon",
            "Name": "weapon_deagle",
            "GiveType": "Replace"
        },
        {
            "Type": "Weapon",
            "Name": "weapon_hegrenade"
        },
        "File:Items/DefuseKit"
    ]
}
```

## Параметры

- `Limits` - условия, при выполнении которых предметы будут выданы.
- `Items` - список выдаваемых предметов.

## Примечание

За доступные типы предметов и за их параметры отвечают плагины-расширения для контроллера предемтов.