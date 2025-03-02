#if defined __vipm_core_objects_vip_unit_included
    #endinput
#endif
#define __vipm_core_objects_vip_unit_included

#include <amxmodx>
#include "VipM/Utils"
#include "VipM/ArrayTrieUtils"
#include "VipM/JsonUtils"

#include "VipM/Core/Objects/Modules/Unit"
#include "VipM/Core/Objects/Limits/Unit"

enum _:S_VipUnit {
    Array:VipUnit_Access, // T_LimitUnit[]
    Array:VipUnit_Modules, // T_ModuleUnit[]
}

enum T_VipUnit { Invalid_VipUnit = -1 }

new Array:g_aVipUnits = Invalid_Array;

VipUnit_Init() {
    CallOnce();

    // Пока что лимиты обязательно должны быть перед модулями
    // После поломки интерфейса будет отдельный форвард для лимитов, а пока только для модулей
    LimitUnit_Init();
    ModuleUnit_Init();

    g_aVipUnits = ArrayCreate(S_VipUnit, 1);
}

VipUnit_GetCount() {
    return ArraySizeSafe(g_aVipUnits);
}

static T_VipUnit:VipUnit__Construct(
    const Array:aAccess,
    const Array:aModules
) {
    if (g_aVipUnits == Invalid_Array) {
        abort(AMX_ERR_GENERAL, "Attempt to create vip unit before init.");
        return Invalid_VipUnit;
    }

    new VipUnit[S_VipUnit];

    VipUnit[VipUnit_Access] = aAccess;
    VipUnit[VipUnit_Modules] = aModules;

    return T_VipUnit:ArrayPushArray(g_aVipUnits, VipUnit);
}

// static VipUnit__Get(const T_LimitUnit:iVipUnit, VipUnit[S_VipUnit], const bool:bOrFail = true) {
//     if (_:iVipUnit < 0 || _:iVipUnit >= VipUnit_GetCount()) {
//         if (bOrFail) {
//            abort(0, "Invalid vip unit index (%d).", iVipUnit);
//         }
        
//         return false;
//     }

//     ArrayGetArray(g_aVipUnits, _:iVipUnit, VipUnit);
//     return true;
// }

static any:VipUnit__GetValue(const T_VipUnit:iVipUnit, const iCell) {
    return ArrayGetCell(g_aVipUnits, _:iVipUnit, iCell);
}

VipUnit_CheckUserAccess(const T_VipUnit:iVipUnit, const UserId) {
    return LimitUnit_ExecuteList(
        VipUnit__GetValue(iVipUnit, VipUnit_Access),
        UserId,
        Limit_Exec_OR
    );
}

Array:VipUnit_GetModules(const T_VipUnit:iVipUnit) {
    return VipUnit__GetValue(iVipUnit, VipUnit_Modules);
}

T_VipUnit:VipUnit_Read(const JSON:valueJson) {
    static Trie:cache = Invalid_Trie;
    TrieCreateIfNotCreated(cache);
    
    new JSON:linked, T_VipUnit:cached = Invalid_VipUnit, linkPath[PLATFORM_MAX_PATH];
    if (PCJson_HandleCachedLinkedValue(cache, valueJson, linked, cached, linkPath, charsmax(linkPath)) == PCJson_CachedLinkedValue_Cached) {
        return cached;
    }
    
    new Array:aAccess = JsonObject_GetLimitUnits(linked, "Access");
    if (aAccess == Invalid_Array) {
        PCJson_ErrorForFile(linked, "Field 'Access' is required in vip object.");
        PCJson_FreeLinked(linked);
        return Invalid_VipUnit;
    }

    new Array:aModules = JsonObject_GetModuleUnits(linked, "Modules");
    if (aModules == Invalid_Array) {
        PCJson_ErrorForFile(linked, "Field 'Modules' is required in vip object.");
        PCJson_FreeLinked(linked);
        return Invalid_VipUnit;
    }

    cached = VipUnit__Construct(aAccess, aModules);
    if (linkPath[0] != EOS && cached != Invalid_VipUnit) {
        TrieSetCell(cache, linkPath, cached);
    }

    PCJson_FreeLinked(linked);
    return cached;
}

Array:VipUnit_ReadList(const JSON:jVipUnits, &Array:aVipUnits = Invalid_Array) {
    ArrayCreateIfNotCreated(aVipUnits, 1, 1);

    if (!json_is_array(jVipUnits)) {
        new T_VipUnit:iVipUnit = VipUnit_Read(jVipUnits);
        if (iVipUnit != Invalid_VipUnit) {
            ArrayPushCell(aVipUnits, iVipUnit);    
        }
    } else {
        json_array_foreach_value (jVipUnits: i => jVipUnit) {
            aVipUnits = VipUnit_ReadList(jVipUnit, aVipUnits);
            json_free(jVipUnit);
        }
    }

    return aVipUnits;
}
