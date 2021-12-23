#include <amxmodx>
#include "VipM/ArrayMap.inc"
#include "VipM/Utils.inc"

enum _:S_Module{
    Module_PluginId,
    Module_Name[32],
    bool:Module_Once,
    bool:Module_Enabled,

    Array:Module_Params, // S_CfgParam
    Trie:Module_Events,
}

#define MODULE_EXISTS(%1) \
    ArrayMapHasKey(Modules, %1)

#define GET_MODULE(%1,%2) \
    ArrayMapGetArray(Modules, %1, %2)

#define GET_MODULE_ID(%1) \
    ArrayMapGetIndex(Modules, %1)

#define GET_MODULE_BY_ID(%1,%2) \
    ArrayMapGetiArray(Modules, %1, %2)

#define SET_MODULE(%1) \
    ArrayMapSetArray(Modules, %1[Module_Name], %1)

// EMIT_MODULE_EVENT(Module, E_ModuleEvent:Event, Return, ...Params)
#define EMIT_MODULE_EVENT(%1,%2,%3) CompositeMacros( \
    if( \
        %1[Module_Events] != Invalid_Trie \
        && TrieKeyExists(%1[Module_Events], IntToStr(%2)) \
    ){ \
        new ___EVENT_FWD; \
        TrieGetCell(%1[Module_Events], IntToStr(%2), ___EVENT_FWD); \
        ExecuteForward(___EVENT_FWD, %3); \
    } \
)

// SET_MODULE_EVENT(Module, E_ModuleEvent:Event, FwdId)
#define SET_MODULE_EVENT(%1,%2,%3) CompositeMacros( \
    if(%3 >= 0){ \
        if(%1[Module_Events] == Invalid_Trie) \
            %1[Module_Events] = TrieCreate(); \
            \
        TrieSetCell(%1[Module_Events], IntToStr(%2), %3); \
    } \
    else TrieDeleteKey(%1[Module_Events], IntToStr(%2)); \
)

new ArrayMap(Modules); // S_Module

Modules_InitModules(){
    InitArrayMap(Modules, S_Module, 8);
}