#include <amxmodx>
#include "VipM/ArrayMap"
// #include "VipM/ItemsController/Objects/Type"

SrvCmds_Init() {
    register_srvcmd("vipm_ic_types", "@SrvCmd_Types");
}

@SrvCmd_Types() {
    PrintTypesData(Types);
}

PrintTypesData(const ArrayMap(am)) {
    server_print("╔═════╤══════════════════════════════════╗");
    server_print("║  #  │ Type name                        ║");
    server_print("╟─────┼──────────────────────────────────╢");
    ArrayMapForeachArray (am: i => Type[S_ItemType]) {
        server_print("║ %03d │ %-32s ║", i, Type[ItemType_Name]);
    }
    server_print("╚═════╧══════════════════════════════════╝");
}