#include <amxmodx>
#include <json>
#include "VipM/ArrayMap"
#include "VipM/DebugMode"

const MODULE_UNITS_PRESERVED_SIZE = 8;
new const MODULE_PARAMS_ISTEMP_KEY[] = "__IsTemp";

enum T_ModuleUnit { Invalid_ModuleUnit = -1 }

enum _:S_ModuleUnit {
    ModuleUnit_ModuleId,
    Trie:ModuleUnit_Params,
}

new Array:ModuleUnits;

#define ModuleUnits_Push(%1) \
    T_ModuleUnit:ArrayPushArray(ModuleUnits, %1)

#define ModuleUnits_Get(%1,%2) \
    ArrayGetArray(ModuleUnits, _:%1, %2)

// Init

Modules_InitUnits() {
    ModuleUnits = ArrayCreate(S_ModuleUnit, MODULE_UNITS_PRESERVED_SIZE);
}


// Actions

ModuleUnits_ResetUser(const UserId) {
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

ModuleUnits_AddListToUser(const UserId, const Array:aUnits) {
    if (aUnits == Invalid_Array) {
        return;
    }
    
    for (new i = 0; i < ArraySize(aUnits); i++) {
        ModuleUnits_AddToUser(UserId, T_ModuleUnit:ArrayGetCell(aUnits, i));
    }
}

ModuleUnits_AddToUser(const UserId, const T_ModuleUnit:iMUnit) {
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

        if (IS_DEBUG) {
            // А то мешает когда в консоль срёт такими сообщениями для каждого бота
            if (!is_user_bot(UserId)) {
                Dbg_Log("ModuleUnits_AddToUser(#%d, '%s'): First add", UserId, Module[Module_Name]);
            }
        }
    }
}


// Readers

bool:ModuleUnits_ReadFromJson(const JSON:jMUnit, sMUnit[S_ModuleUnit]) {
    new ModuleName[32];
    json_object_get_string(jMUnit, "Module", ModuleName, charsmax(ModuleName));
    json_object_remove(jMUnit, "Module");

    if (!MODULE_EXISTS(ModuleName)) {
        Json_LogForFile(jMUnit, "WARNING", "Module `%s` not found.", ModuleName);
        return false;
    }

    new iModule = GET_MODULE_ID(ModuleName);
    // Modules_Enable(iModule);

    new sModule[S_Module];
    GET_MODULE_BY_ID(iModule, sModule);

    // Пусть юниты модулей читаются вне зависимости от состояния модуля
    // Чтобы позже можно было на лету переключать состояние
    
    sMUnit[ModuleUnit_ModuleId] = iModule;
    sMUnit[ModuleUnit_Params] = TrieCreate();
    
    new ErrParam[32];
    if (!CfgParam_ReadList(jMUnit, sMUnit[ModuleUnit_Params], sModule[Module_Params], ErrParam, charsmax(ErrParam))) {
        Json_LogForFile(jMUnit, "WARNING", "Param `%s` required for module `%s`, but not found.", ErrParam, sModule[Module_Name]);
        return false;
    }

    Forwards_DefaultReturn(VIPM_CONTINUE);
    if (
        Forwards_CallP("ReadUnit", jMUnit, sMUnit[ModuleUnit_Params]) == VIPM_STOP
        || Forwards_CallP("ReadModuleUnit", jMUnit, sMUnit[ModuleUnit_Params]) == VIPM_STOP
    ) {
        TrieDestroySafe(sMUnit[ModuleUnit_Params]);
        return false;
    }
    
    new ret;
    EMIT_MODULE_EVENT(sModule, Module_OnRead, ret, jMUnit, sMUnit[ModuleUnit_Params]);

    if (ret == VIPM_STOP) {
        TrieDestroySafe(sMUnit[ModuleUnit_Params]);
        return false;
    }

    if (!sModule[Module_Used]) {
        sModule[Module_Used] = true;
        SET_MODULE(sModule);
        Dbg_Log("Module `%s` marked as used.", ModuleName);
    }

    return true;
}

// Loaders

T_ModuleUnit:ModuleUnits_LoadFromJson(const JSON:jModuleUnit) {
    static Trie:tCache = Invalid_Trie;
    TrieCreateIfNotCreated(tCache);

    new JSON:jRefed, T_ModuleUnit:iUnit = Invalid_ModuleUnit;
    Json_GetCachedRefValue_Begin(jModuleUnit, tCache, iUnit, jRefed)
    {
        new ModuleUnit[S_ModuleUnit];
        if (ModuleUnits_ReadFromJson(jRefed, ModuleUnit)) {
            iUnit = ModuleUnits_Push(ModuleUnit);
        }
    }
    Json_GetCachedRefValue_End(jModuleUnit, tCache, iUnit, jRefed)
}

Array:ModuleUnits_LoadListFromJson(const JSON:jModules, &Array:aModules = Invalid_Array) {
    ArrayCreateIfNotCreated(aModules, 1, 1);

    if (!json_is_array(jModules)) {
        new T_ModuleUnit:iMUnit = ModuleUnits_LoadFromJson(jModules);
        if (iMUnit != Invalid_ModuleUnit) {
            ArrayPushCell(aModules, iMUnit);    
        }
    } else {
        json_array_foreach_value (jModules: i => jModule) {
            aModules = ModuleUnits_LoadListFromJson(jModule, aModules);
            json_free(jModule);
        }
    }

    return aModules;
}
