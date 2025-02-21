#include <amxmodx>
#include <ItemsController>
#include "VipM/Utils"
#include "VipM/Forwards"

public stock const PluginName[] = "Items Controller";
public stock const PluginVersion[] = IC_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://github.com/ArKaNeMaN/amxx-VipModular-pub";
public stock const PluginDescription[] = "Unified interface for items giving.";

#include "ItemsController/Objects/Items/Instance"
#include "ItemsController/DefaultObjects/Regsitrar"

public plugin_precache() {
    PluginInit();
}

PluginInit() {
    CallOnce();

    register_plugin(PluginName, PluginVersion, PluginAuthor);
    register_library(IC_LIBRARY);
    CreateConstCvar(IC_VERSION_CVAR, IC_VERSION);
    Forwards_Init();
    
    ItemInstance_Init();
    DefaultObjects_RegisterAll();
    Forwards_RegAndCall("VipM_IC_OnInitTypes", ET_IGNORE); // deprecated
}

#include "ItemsController/API/Items"
#include "ItemsController/API/Compat"

public plugin_natives() {
    set_native_filter("@NativeFilter");

    register_native("IC_Init", "@_Init");
    register_native("VipM_IC_Init", "@_Init"); // deprecated

    API_Items_Register();
    API_Compat_Register();
}

@_Init() {
    PluginInit();
}

@NativeFilter(const name[], index, trap) {
    if (DefaultObjects_ItemType_CustomWeapon_NativeFilter(name, trap)) {
        return PLUGIN_HANDLED;
    }

    return PLUGIN_CONTINUE;
}
