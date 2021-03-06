# Контроллер предметов

Колнтроллер предметов (`VipM-ItemsController`) отвечает за регистрацию, загрузку и выдачу предметов во всей системе привилегий.

## Структура обьекта предмета

```jsonc
{
    "Type": "<ItemType>",
    "Name": "<ItemName>",

    "<Param1>": "",
    /* ... */
    "<ParamN>": []
}
```

- `<ItemType>` - название типа предмета. Определяется плагином-расширением, регистрирующим тип предмета.
- `<ParamX>` - какой-либо параметр предмета. Названия и типы параметров определяются плагином-расширением, добавляющим тип предмета.
- Поле `Name` необязательно, зависит от типа предмета.

## Для авторов расширений предметов

У предметов нет списка параметров, в отличии от модулей и ограничений. Все параметры читаются руками в событии `OnRead`.

Поле `Name`, при наличии, будет прочитано ДО вызова `OnRead`. Если оно не требуется, можно его удалить из параметров в этом событии (`TrieDeleteKey(Params, "Name")`).

## Использование в сторонних плагинах

Допускается использование контроллера предметов в плагинах, не относящихся к системе привилегий. Для работы контроллера не требуется запущенное ядро системы привилегий.
