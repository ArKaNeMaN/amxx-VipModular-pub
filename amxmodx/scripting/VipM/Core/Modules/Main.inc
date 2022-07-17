#if defined _vipmodular_src_Modules_included
    #endinput
#endif
#define _vipmodular_src_Modules_included

#define NATIVE_CHECK_MODULE(%1) \
    if (!MODULE_EXISTS(%1)) {\
        log_error(1, "[ERROR] Module '%s' not found.", %1);\
    }

#include "VipM/Core/Modules/Modules.inc"
#include "VipM/Core/Modules/Units.inc"

Modules_Init() {
    Modules_InitModules();
    Modules_InitUnits();
}
