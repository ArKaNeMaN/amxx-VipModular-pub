#include amxmodx

#define GET_ITEM(%1,%2) \
    ArrayGetArray(Items, %1, %2)

#define ADD_ITEM(%1) \
    ArrayPushArray(Items, %1)

#define ITEM_EXISTS(%1) \
    (%1 >= 0 && %1 < ArraySizeSafe(Items))


#define TYPE_EXISTS(%1) \
    ArrayMapHasKey(Types, %1)

#define GET_TYPE(%1,%2) \
    ArrayMapGetArray(Types, %1, %2)

#define GET_TYPE_ID(%1) \
    ArrayMapGetIndex(Types, %1)

#define GET_TYPE_BY_ID(%1,%2) \
    ArrayMapGetiArray(Types, %1, %2)

#define GET_ITEM_TYPE(%1,%2) \
    GET_TYPE_BY_ID(%1[Item_Type], %2)

#define SET_TYPE(%1) \
    ArrayMapSetArray(Types, %1[ItemType_Name], %1)

#define ADD_TYPE(%1) \
    ArrayMapPushArray(Types, %1, %1[ItemType_Name])


// EMIT_TYPE_EVENT(Type, E_ItemTypeEvent:Event, Return, ...Params)
#define EMIT_TYPE_EVENT(%1,%2,%3) \
    if(\
        %1[ItemType_Events] != Invalid_Trie\
        && TrieKeyExists(%1[ItemType_Events], IntToStr(%2))\
    ){\
        new ___EVENT_FWD;\
        TrieGetCell(%1[ItemType_Events], IntToStr(%2), ___EVENT_FWD);\
        ExecuteForward(___EVENT_FWD, %3);\
    }

// SET_TYPE_EVENT(Type, E_ItemTypeEvent:Event, FwdId)
#define SET_TYPE_EVENT(%1,%2,%3) \
    if(%3 >= 0){\
        if(%1[ItemType_Events] == Invalid_Trie)\
            %1[ItemType_Events] = TrieCreate();\
        TrieSetCell(%1[ItemType_Events], IntToStr(%2), %3);\
    }\
    else TrieDeleteKey(%1[ItemType_Events], IntToStr(%2))


PrintTypesData(const am[ArrayMap]){
    server_print("╔═════╤══════════════════════════════════╗");
    server_print("║  #  │ Type name                        ║");
    server_print("╟─────┼──────────────────────────────────╢");
    ArrayMapForeachArray(am => Type[S_ItemType])
        server_print("║ %03d │ %-32s ║", __i__, Type[ItemType_Name]);
    server_print("╚═════╧══════════════════════════════════╝");
}