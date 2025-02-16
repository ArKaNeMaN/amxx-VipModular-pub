#if defined __vipm_core_objects_modules_unit_included
    #endinput
#endif
#define __vipm_core_objects_modules_unit_included

#include <amxmodx>
#include "VipM/Utils"
#include <VipM/Modules>
#include "VipM/ArrayMap"
#include "VipM/ArrayTrieUtils"

#include "VipM/Core/Objects/Modules/Type"

enum T_ModuleUnit { Invalid_ModuleUnit = -1 }

enum _:S_ModuleUnit {
    T_ModuleType:ModuleUnit_Type,
    Trie:ModuleUnit_Params,
}

static Array:g_aModuleUnits = Invalid_Array;

ModuleUnit_Init() {
    CallOnce();

    ModuleType_Init();

    g_aModuleUnits = ArrayCreate(S_ModuleUnit, 1);
    Forwards_Reg("ReadModuleUnit", ET_IGNORE, FP_CELL, FP_CELL); // TODO: call
    Forwards_RegAndCall("InitModules", ET_IGNORE);
}

ModuleUnit_GetCount() {
    return ArraySizeSafe(g_aModuleUnits);
}

static T_ModuleUnit:ModuleUnit__Construct(const T_ModuleType:iModuleType, const Trie:tParams) {
    if (g_aModuleUnits == Invalid_Array) {
        abort(AMX_ERR_GENERAL, "Attempt to create module unit before init.");
        return Invalid_ModuleUnit;
    }
    
    new ModuleUnit[S_ModuleUnit];

    ModuleUnit[ModuleUnit_Type] = iModuleType;
    ModuleUnit[ModuleUnit_Params] = tParams;

    ModuleType_MarkAsUsed(iModuleType);

    return T_ModuleUnit:ArrayPushArray(g_aModuleUnits, ModuleUnit);
}

// static ModuleUnit__Get(const T_ModuleUnit:iModuleUnit, ModuleUnit[S_ModuleUnit]) {
//     ArrayGetArray(g_aModuleUnits, _:iModuleUnit, ModuleUnit);
// }

static any:ModuleType__GetValue(const T_ModuleUnit:iModuleUnit, const iCell) {
    return ArrayGetCell(g_aModuleUnits, _:iModuleUnit, iCell);
}

static T_ModuleType:ModuleUnit__ReadType(const JSON:jModuleUnit) {
    new sModuleTypeName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
    Json_Object_GetString(jModuleUnit, "Module", sModuleTypeName, charsmax(sModuleTypeName));
    trim(sModuleTypeName);

    if (!sModuleTypeName[0]) {
        Json_ErrorForFile(jModuleUnit, "Module type name not specified.");
        return Invalid_ModuleType;
    }

    new T_ModuleType:iModuleType = ModuleType_Find(sModuleTypeName);

    if (iModuleType == Invalid_ModuleType) {
        Json_ErrorForFile(jModuleUnit, "Module type '%s' not found.", sModuleTypeName);
        return Invalid_ModuleType;
    }

    return iModuleType;
}

T_ModuleUnit:ModuleUnit_Read(const JSON:jModuleUnit) {
    static Trie:tCache = Invalid_Trie;
    TrieCreateIfNotCreated(tCache);

    new JSON:jRefed, T_ModuleUnit:iModuleUnit = Invalid_ModuleUnit;
    Json_GetCachedRefValue_Begin(jModuleUnit, tCache, iModuleUnit, jRefed)
    {
        new T_ModuleType:iModuleType = ModuleUnit__ReadType(jRefed);
        new Trie:tParams = Invalid_Trie;

        if (iModuleType != Invalid_ModuleType) {
            tParams = ModuleType_ReadParams(iModuleType, jRefed);
        }

        if (tParams != Invalid_Trie) {
            iModuleUnit = ModuleUnit__Construct(iModuleType, tParams);
        }
    }
    Json_GetCachedRefValue_End(jModuleUnit, tCache, iModuleUnit, jRefed)
}

Array:ModuleUnit_ReadList(const JSON:jModuleUnits, &Array:aModuleUnits = Invalid_Array) {
    ArrayCreateIfNotCreated(aModuleUnits, 1, 1);

    if (!json_is_array(jModuleUnits)) {
        new T_ModuleUnit:iModuleUnit = ModuleUnit_Read(jModuleUnits);
        if (iModuleUnit != Invalid_ModuleUnit) {
            ArrayPushCell(aModuleUnits, iModuleUnit);    
        }
    } else {
        json_array_foreach_value (jModuleUnits: i => jModuleUnit) {
            aModuleUnits = ModuleUnit_ReadList(jModuleUnit, aModuleUnits);
            json_free(jModuleUnit);
        }
    }

    return aModuleUnits;
}

Array:JsonObject_GetModuleUnits(const JSON:jObj, const sKey[], const bool:bDotNot = false, &Array:aModuleUnits = Invalid_Array) {
    if (!json_object_has_value(jObj, sKey, .dot_not = bDotNot)) {
        return aModuleUnits;
    }

    new JSON:jModuleUnits = json_object_get_value(jObj, sKey, bDotNot);
    aModuleUnits = ModuleUnit_ReadList(jModuleUnits, aModuleUnits);
    json_free(jModuleUnits);

    return aModuleUnits;
}

Trie:ModuleUnit_GetParams(const T_ModuleUnit:iModuleUnit) {
    return ModuleType__GetValue(iModuleUnit, ModuleUnit_Params);
}

ModuleUnit_GetTypeName(const T_ModuleUnit:iModuleUnit, sOut[], const iOutLen) {
    return ModuleType_GetName(ModuleType__GetValue(iModuleUnit, ModuleUnit_Type), sOut, iOutLen);
}

// ModuleUnit_iGetTypeName(const T_ModuleUnit:iModuleUnit) {
//     new sName[VIPM_MODULES_TYPE_NAME_MAX_LEN];
//     ModuleUnit_GetModuleName(iModuleUnit, sName, charsmax(sName));
//     return sName;
// }

Trie:ModuleUnit_FreeParamsIfTemp(&Trie:tParams) {
    if (tParams == Invalid_Trie) {
        return tParams;
    }

    if (TrieKeyExists(tParams, VIPM_MODULES_PARAMS_TEMP_MARK_KEY)) {
        TrieDestroy(tParams);
    }

    return tParams;
}

Trie:ModuleUnit_Merge(const T_ModuleUnit:iModuleUnit, const Trie:tParams1, const Trie:tParams2) {
    return ModuleType_MergeParams(ModuleType__GetValue(iModuleUnit, ModuleUnit_Type), tParams1, tParams2);
}
