#include <amxmodx>
#include <json>
#include "VipM/ArrayMap"
#include "VipM/DebugMode"

const MODULE_UNITS_PRESERVED_SIZE = 8;
new const MODULE_PARAMS_ISTEMP_KEY[] = "__IsTemp";

enum T_ModuleUnit { Invalid_ModuleUnit = -1 }

enum _:S_ModuleUnit{
    ModuleUnit_ModuleId,
    Trie:ModuleUnit_Params,
}

new Array:ModuleUnits;

#define ModuleUnits_Push(%1) \
    T_ModuleUnit:ArrayPushArray(ModuleUnits, %1)

#define ModuleUnits_Get(%1,%2) \
    ArrayGetArray(ModuleUnits, _:%1, %2)


// Cache

new Trie:__cache_ModuleUnits;

T_ModuleUnit:ModuleUnits_AddToCache(const T_ModuleUnit:iMUnit, const CacheKey[] = ""){
    if (CacheKey[0]) {
        TrieSetCell(__cache_ModuleUnits, CacheKey, iMUnit);
    }

    return iMUnit;
}

T_ModuleUnit:ModuleUnits_GetFromCache(const CacheKey[]){
    new T_ModuleUnit:iMUnit;
    return TrieGetCell(__cache_ModuleUnits, CacheKey, iMUnit)
        ? iMUnit
        : Invalid_ModuleUnit;
}

#define ModuleUnits_CacheExists(%1) \
    TrieKeyExists(__cache_ModuleUnits, %1)


// Init

Modules_InitUnits(){
    ModuleUnits = ArrayCreate(S_ModuleUnit, MODULE_UNITS_PRESERVED_SIZE);
    __cache_ModuleUnits = TrieCreate();
}


// Actions

ModuleUnits_ResetUser(const UserId){
    if (gUserVip[UserId] == Invalid_Trie) {
        return;
    }

    new TrieIter:Iter = TrieIterCreate(gUserVip[UserId]);
    while (!TrieIterEnded(Iter)) {
        new Trie:Params = Invalid_Trie;
        if (
            TrieIterGetCell(Iter, Params)
            && TrieKeyExists(Params, MODULE_PARAMS_ISTEMP_KEY)
            && Params != Invalid_Trie
        ) {
            TrieDestroy(Params);
        }

        TrieIterNext(Iter);
    }
    TrieIterDestroy(Iter);

    TrieDestroy(gUserVip[UserId]);
}

ModuleUnits_AddListToUser(const UserId, const Array:aUnits){
    if (aUnits == Invalid_Array) {
        return;
    }
    
    for (new i = 0; i < ArraySize(aUnits); i++) {
        ModuleUnits_AddToUser(UserId, T_ModuleUnit:ArrayGetCell(aUnits, i));
    }
}

ModuleUnits_AddToUser(const UserId, const T_ModuleUnit:iMUnit){
    if (gUserVip[UserId] == Invalid_Trie) {
        gUserVip[UserId] = TrieCreate();
    }

    new Unit[S_ModuleUnit];
    ModuleUnits_Get(iMUnit, Unit);

    new Module[S_Module];
    GET_MODULE_BY_ID(Unit[ModuleUnit_ModuleId], Module);

    if (TrieKeyExists(gUserVip[UserId], Module[Module_Name])) {
        if (Module[Module_Once]) {
            return;
        }

        new Trie:Params;
        TrieGetCell(gUserVip[UserId], Module[Module_Name], Params);

        new Trie:Ret = Invalid_Trie;
        EMIT_MODULE_EVENT(Module, Module_OnCompareParams, _:Ret, Params, Unit[ModuleUnit_Params]);
        if (Ret == Invalid_Trie) {
            return;
        }

        if (TrieKeyExists(Params, MODULE_PARAMS_ISTEMP_KEY)) {
            TrieDestroy(Params);
        }

        TrieSetCell(Ret, MODULE_PARAMS_ISTEMP_KEY, true);
        TrieSetCell(gUserVip[UserId], Module[Module_Name], Ret);
        Dbg_Log("ModuleUnits_AddToUser(#%d, '%s'): Compared", UserId, Module[Module_Name]);
    } else {
        TrieSetCell(gUserVip[UserId], Module[Module_Name], Unit[ModuleUnit_Params], false);
        Dbg_Log("ModuleUnits_AddToUser(#%d, '%s'): First add", UserId, Module[Module_Name]);
    }
}


// Readers

bool:ModuleUnits_ReadFromJson(const JSON:jMUnit, sMUnit[S_ModuleUnit]){
    new ModuleName[32];
    json_object_get_string(jMUnit, "Module", ModuleName, charsmax(ModuleName));
    json_object_remove(jMUnit, "Module");

    if (!MODULE_EXISTS(ModuleName)) {
        log_amx("[WARNING] Module `%s` not found.", ModuleName);
        return false;
    }

    new sModule[S_Module];
    GET_MODULE(ModuleName, sModule);
    
    sMUnit[ModuleUnit_ModuleId] = GET_MODULE_ID(ModuleName);
    sMUnit[ModuleUnit_Params] = TrieCreate();
    
    new ErrParam[32];
    if (!Cfg_ReadParams(jMUnit, sMUnit[ModuleUnit_Params], sModule[Module_Params], ErrParam, charsmax(ErrParam))) {
        log_amx("[WARNING] Param `%s` required for module `%s`, but not found.", ErrParam, sModule[Module_Name]);
        return false;
    }
    
    new ret;
    EMIT_MODULE_EVENT(sModule, Module_OnRead, ret, jMUnit, sMUnit[ModuleUnit_Params]);

    if (ret == VIPM_STOP) {
        TrieDestroySafe(sMUnit[ModuleUnit_Params]);
        return false;
    }

    return true;
}


// Loaders

T_ModuleUnit:ModuleUnits_LoadFromFile(const FileName[]){
    if (ModuleUnits_CacheExists(FileName)) {
        return ModuleUnits_GetFromCache(FileName);
    }
    
    if (!JSON_FILE_EXTSTS(FileName)) {
        log_amx("[WARNING] File `%s` not found.", FileName);
        return Invalid_ModuleUnit;
    }

    new JSON:jMUnit = GET_FILE_JSON(FileName);
    new T_ModuleUnit:iModuleUnit = ModuleUnits_LoadFromJson(jMUnit);
    json_free(jMUnit);

    return ModuleUnits_AddToCache(iModuleUnit, FileName);
}

T_ModuleUnit:ModuleUnits_LoadFromJson(const JSON:jModuleUnit){
    if (jModuleUnit == Invalid_JSON) {
        return Invalid_ModuleUnit;
    }

    new Ref[128];
    if (Json_IsRef(jModuleUnit, Ref, charsmax(Ref))) {
        return ModuleUnits_LoadFromFile(Ref);
    }

    new sModuleUnit[S_ModuleUnit];
    if (ModuleUnits_ReadFromJson(jModuleUnit, sModuleUnit)) {
        return ModuleUnits_Push(sModuleUnit);
    }
    
    return Invalid_ModuleUnit;
}

Array:ModuleUnits_LoadListFromJson(const JSON:jModules, Array:aModules = Invalid_Array){
    new T_ModuleUnit:iMUnit;
    ArrayCreateIfNotCreated(aModules, 1, json_get_count(jModules));

    if (!json_is_array(jModules)) {
        iMUnit = ModuleUnits_LoadFromJson(jModules);
        if (iMUnit != Invalid_ModuleUnit) {
            ArrayPushCell(aModules, iMUnit);    
        }
    } else {
        json_array_foreach_value(jModules: i => jModule) {
            iMUnit = ModuleUnits_LoadFromJson(jModule);
            if (iMUnit != Invalid_ModuleUnit) {
                ArrayPushCell(aModules, iMUnit);
            }
        }
    }

    ArrayDestroyIfEmpty(aModules);
    return aModules;
}
