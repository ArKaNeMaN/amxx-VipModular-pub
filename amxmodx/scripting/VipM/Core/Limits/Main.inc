#if defined _vipmodular_src_Limits_included
    #endinput
#endif
#define _vipmodular_src_Limits_included

#include amxmodx
#include json

#define NATIVE_CHECK_LIMIT_TYPE(%1) \
    if(!LIMIT_TYPE_EXISTS(%1)){ \
        log_error(0, "[ERROR] Limit type '%s' not found.", %1); \
    }

#include "VipM/Core/Limits/Types.inc"
#include "VipM/Core/Limits/Units.inc"

Limits_Init(){
    InitArrayMap(Limits, S_Limit, 8);

    LimitUnits = ArrayCreate(S_LimitUnit, 16);
    __cache_LimitUnits = TrieCreate();
}
