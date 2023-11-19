
enum _:S_WeaponMenu{
    WeaponMenu_Name[64],
    WeaponMenu_Title[128],
    Array:WeaponMenu_Items,
    Array:WeaponMenu_Limits,
    WeaponMenu_Count,
    bool:WeaponMenu_BackOnExit,
    bool:WeaponMenu_Fake,
    WeaponMenu_FakeMessage[256],
    WeaponMenu_PerPage,
}

enum MenuItemType{
    MenuItemType_Default = 0,
    MenuItemType_Text,
    MenuItemType_Blank,
}

enum _:S_MenuItem{
    MenuItem_Title[64],
    Array:MenuItem_Items,
    bool:MenuItem_UseCounter,
    MenuItemType:MenuItem_Type,
    bool:MenuItem_FakeInactive,
    
    Array:MenuItem_ShowLimits,
    Array:MenuItem_ActiveLimits,
    Array:MenuItem_Limits,
}
