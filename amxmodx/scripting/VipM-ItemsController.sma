#include <amxmodx>
#include <VipModular>
#include "VipM/ArrayTrieUtils"
#include "VipM/ArrayMap"
#include "VipM/Utils"
#include "VipM/Forwards"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM] Items Controller";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Vip Modular`s items controller";

#include "VipM/ItemsController/Structs"
#include "VipM/ItemsController/Utils"

DefineArrayMap(Types); // S_ItemType
new Array:Items;

public plugin_precache() {
    PluginInit();
}

public VipM_OnInitModules() {
    PluginInit();
}

PluginInit() {
    CallOnce();

    RegisterPluginByVars();
    CreateConstCvar("vipm_ic_version", PluginVersion);
    SrvCmds_Init();

    Forwards_Init("VipM_IC");
    Forwards_Reg("GiveItem", ET_STOP, FP_CELL, FP_CELL);
    Forwards_Reg("ReadItem", ET_STOP, FP_CELL, FP_CELL);

    InitArrayMap(Types, S_ItemType, 8);
    Forwards_RegAndCall("InitTypes", ET_IGNORE);

    Items = ArrayCreate(S_Item, 16);
    Forwards_RegAndCall("Loaded", ET_IGNORE);
}

#include "VipM/ItemsController/Natives"
#include "VipM/ItemsController/Configs"
#include "VipM/ItemsController/SrvCmds"
