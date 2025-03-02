#if defined __vipm_core_objects_modules_type_included
    #endinput
#endif
#define __vipm_core_objects_modules_type_included

#include <amxmodx>
#include <VipM/Modules>
#include <VipM/Params>
#include "VipM/Forwards"
#include "VipM/Utils"
#include "VipM/ArrayMap"
#include "VipM/ArrayTrieUtils"
#include "VipM/Core/Objects/Param"

enum _:S_ModuleType {
    ModuleType_Name[VIPM_MODULES_TYPE_NAME_MAX_LEN],
    Array:ModuleType_Params, // S_CfgParam[]

    bool:ModuleType_Used,
    bool:ModuleType_Active,

    ModuleType_Events[E_ModuleEvent],
}

enum T_ModuleType { Invalid_ModuleType = -1 }

static ArrayMap(g_amModuleTypes); // S_ModuleType

ModuleType_Init() {
    CallOnce();

    ArrayMapCreate(g_amModuleTypes, S_ModuleType);
    
    Forwards_Reg("ActivateModule", ET_STOP, FP_STRING);
}

Array:ModuleType_GetAll() {
    return ArrayClone(g_amModuleTypes[AM_Arr]);
}

ModuleType_GetCount() {
    return ArrayMapSize(g_amModuleTypes);
}

T_ModuleType:ModuleType_Construct(const sName[]) {
    if (!ArrayMapCreated(g_amModuleTypes)) {
        abort(AMX_ERR_GENERAL, "Attempt to create module type before modules init.");
        return Invalid_ModuleType;
    }

    if (ModuleType_Find(sName) != Invalid_ModuleType) {
        abort(AMX_ERR_PARAMS, "Module type '%s' already exists.", sName);
        return Invalid_ModuleType;
    }

    new ModuleType[S_ModuleType];

    copy(ModuleType[ModuleType_Name], charsmax(ModuleType[ModuleType_Name]), sName);
    ModuleType[ModuleType_Params] = Invalid_Array;

    ModuleType[ModuleType_Used] = false;
    ModuleType[ModuleType_Active] = false;

    arrayset(ModuleType[ModuleType_Events], -1, sizeof(ModuleType[ModuleType_Events]));

    return T_ModuleType:ArrayMapPushArray(g_amModuleTypes, ModuleType, ModuleType[ModuleType_Name]);
}

T_ModuleType:ModuleType_Find(const sName[]) {
    if (ArrayMapKeyExists(g_amModuleTypes, sName)) {
        return T_ModuleType:ArrayMapGetIndex(g_amModuleTypes, sName);
    } else {
        return Invalid_ModuleType;
    }
}

static ModuleType__Get(const T_ModuleType:iModuleType, ModuleType[S_ModuleType]) {
    ArrayMapGetArray(g_amModuleTypes, _:iModuleType, ModuleType);
}

static ModuleType__Set(const ModuleType[S_ModuleType]) {
    ArrayMapSetArrayByKey(g_amModuleTypes, ModuleType[ModuleType_Name], ModuleType);
}

static ModuleType__SetValue(const T_ModuleType:iModuleType, const iCell, const any:iValue) {
    return ArrayMapSetCell(g_amModuleTypes, iModuleType, iValue, iCell);
}

static any:ModuleType__GetValue(const T_ModuleType:iModuleType, const iCell) {
    return ArrayMapGetCell(g_amModuleTypes, iModuleType, iCell);
}

ModuleType_GetName(const T_ModuleType:iModuleType, sOut[], const iOutLen) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    return copy(sOut, iOutLen, ModuleType[ModuleType_Name]);
}

ModuleType_iGetName(const T_ModuleType:iModuleType) {
    new sName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    ModuleType_GetName(iModuleType, sName, charsmax(sName));
    return sName;
}

bool:ModuleType_IsActive(const T_ModuleType:iModuleType) {
    return ModuleType__GetValue(iModuleType, ModuleType_Active);
}

Trie:ModuleType_ReadParams(const T_ModuleType:iModuleType, const JSON:jParams) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    new Trie:tParams = TrieCreate();

    new sErrParam[VIPM_PARAM_NAME_MAX_LEN];
    if (!CfgParam_ReadList(jParams, tParams, ModuleType[ModuleType_Params], sErrParam, charsmax(sErrParam))) {
        PCJson_ErrorForFile(jParams, "Param '%s' required for '%s' module.", sErrParam, ModuleType[ModuleType_Name]);
        TrieDestroy(tParams);
        return Invalid_Trie;
    }

    if (ModuleType[ModuleType_Events][Module_OnRead] >= 0) {
        new iRet = VIPM_STOP;
        ExecuteForward(ModuleType[ModuleType_Events][Module_OnRead], iRet, jParams, tParams);

        if (iRet == VIPM_STOP) {
            TrieDestroy(tParams);
            return Invalid_Trie;
        }
    }
    
    return tParams;
}

Array:ModuleType_GetParams(const T_ModuleType:iModuleType) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    if (ModuleType[ModuleType_Params] == Invalid_Array) {
        return Invalid_Array;
    }

    return ArrayClone(ModuleType[ModuleType_Params]);
}

ModuleType_MergeParamsList(const T_ModuleType:iModuleType, const Array:aParams) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    ModuleType[ModuleType_Params] = CfgParam_Merge(ModuleType[ModuleType_Params], aParams);

    ModuleType__Set(ModuleType);
}

ModuleType_SetEventListener(
    const T_ModuleType:iModuleType,
    const E_ModuleEvent:iEvent,
    const PluginId,
    const sFuncName[]
) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    if (iEvent == Module_OnCompareParams) {
        log_amx("[WARNING] Event 'Module_OnCompareParams' is deprecated and will not be fired.");
        return;
    }

    if (ModuleType[ModuleType_Events][iEvent] >= 0) {
        DestroyForward(ModuleType[ModuleType_Events][iEvent]);
    }
    
    ModuleType[ModuleType_Events][iEvent] = ModuleType__MakeEventForward(iEvent, PluginId, sFuncName);

    if (ModuleType[ModuleType_Events][iEvent] < 0) {
        abort(AMX_ERR_PARAMS, "[ERROR] Can't create forward for func '%s' in plugin #%d.", sFuncName, PluginId);
        return;
    }

    ModuleType__Set(ModuleType);
}

static ModuleType__MakeEventForward(const E_ModuleEvent:iEvent, const PluginId, const sFuncName[]) {
    switch (iEvent) {
        case Module_OnRead: // (const JSON:jCfg, Trie:Params)
            return CreateOneForward(PluginId, sFuncName, FP_CELL, FP_CELL);

        case Module_OnActivated: // ()
            return CreateOneForward(PluginId, sFuncName);

        case Module_OnMergeParams: // (const Trie:tParams1, const Trie:tParams2)
            return CreateOneForward(PluginId, sFuncName, FP_CELL, FP_CELL);
    }

    abort(AMX_ERR_GENERAL, "[ERROR] Invalid event index (%d).", iEvent);
    return -1;
}

ModuleType_MarkAsUsed(const T_ModuleType:iModuleType, const bool:bState = true) {
    ModuleType__SetValue(iModuleType, ModuleType_Used, bState);
}

bool:ModuleType_Activate(const T_ModuleType:iModuleType) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    if (!ModuleType[ModuleType_Active]) {
        Forwards_DefaultReturn(VIPM_CONTINUE);
        if (Forwards_CallP("ActivateModule", ModuleType[ModuleType_Name]) != VIPM_CONTINUE) {
            return false;
        }

        ModuleType[ModuleType_Active] = true;
        
        if (ModuleType[ModuleType_Events][Module_OnActivated] >= 0) {
            ExecuteForward(ModuleType[ModuleType_Events][Module_OnActivated]);
        }

        ModuleType__Set(ModuleType);
    }

    return ModuleType[ModuleType_Active];
}

ModuleType_ActivateUsed() {
    ArrayMapForeachArray (g_amModuleTypes: iModuleType => ModuleType[S_ModuleType]) {
        if (ModuleType[ModuleType_Used]) {
            ModuleType_Activate(iModuleType);
        }
    }
}

Trie:ModuleType_MergeParams(const T_ModuleType:iModuleType, const Trie:tParams1, const Trie:tParams2) {
    new ModuleType[S_ModuleType];
    ModuleType__Get(iModuleType, ModuleType);

    new Trie:tRes = tParams1;
    if (ModuleType[ModuleType_Events][Module_OnMergeParams] >= 0) {
        ExecuteForward(ModuleType[ModuleType_Events][Module_OnMergeParams], _:tRes, tParams1, tParams2);

        if (tRes != tParams1 && tRes != tParams2) {
            TrieSetCell(tRes, VIPM_MODULES_PARAMS_TEMP_MARK_KEY, true);
        }
    }
    
    return tRes;
}
