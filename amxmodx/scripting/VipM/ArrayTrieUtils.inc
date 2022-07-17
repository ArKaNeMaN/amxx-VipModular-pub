#if defined _vipmodular_src_ArrayTrieUtils_included
    #endinput
#endif
#define _vipmodular_src_ArrayTrieUtils_included

#include amxmodx

#define ArraySizeSafe(%1) \
    (%1 == Invalid_Array ? 0 : ArraySize(%1))

#define TrieSizeSafe(%1) \
    (%1 == Invalid_Trie ? 0 : TrieGetSize(%1))

stock ArrayDestroySafe(Array:a)
    if(a != Invalid_Array)
        ArrayDestroy(a);

stock TrieDestroySafe(Trie:t)
    if(t != Invalid_Trie)
        TrieDestroy(t);

#define ArrayForeachArray(%1=>%2[%3]) \
    ArrayForeachArrayEx(%1 => %2[%3], __i__)

#define ArrayForeachArrayEx(%1=>%2[%3],%4) \
    if(%1 != Invalid_Array)\
        for(new %2[%3], %4 = 0; %4 < ArraySizeSafe(%1); %4++)\
            if(ArrayGetArray(%1, %4, %2))

#define ArrayForeachArray2(%1:%4=>%2[%3]) \
    if(%1 != Invalid_Array) \
        for(new %2[%3], %4 = 0; %4 < ArraySizeSafe(%1); %4++) \
            if(ArrayGetArray(%1, %4, %2))

#define ArrayCreateIfNotCreated(%1,%2) \
    %1 = (%1 == Invalid_Array) \
        ? ArrayCreate(%2) \
        : %1

#define TrieCreateIfNotCreated(%1) \
    %1 = (%1 == Invalid_Trie) \
        ? TrieCreate() \
        : %1

#define ArrayDestroyIfEmpty(%1) \
    CompositeMacros( \
        if(!ArraySizeSafe(%1)) \
            ArrayDestroy(%1); \
    )
