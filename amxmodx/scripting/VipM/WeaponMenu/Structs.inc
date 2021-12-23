
enum _:S_WeaponMenu{
    WeaponMenu_Name[64],
    WeaponMenu_Title[128],
    Array:WeaponMenu_Items,
    WeaponMenu_MinRound,
}

enum MenuItemType{
    MenuItemType_Default = 0,
    MenuItemType_Text,
    MenuItemType_Blank,
}

enum _:S_MenuItem{
    MenuItem_Title[64],
    Array:MenuItem_Items,
    MenuItem_MinRound,
    
    MenuItemType:MenuItem_Type,
}
