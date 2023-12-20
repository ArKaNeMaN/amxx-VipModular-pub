SrvCmds_Init() {
    register_srvcmd("vipm_ic_types", "@SrvCmd_Types");
}

@SrvCmd_Types() {
    PrintTypesData(Types);
}

PrintTypesData(const am[ArrayMap]) {
    server_print("╔═════╤══════════════════════════════════╗");
    server_print("║  #  │ Type name                        ║");
    server_print("╟─────┼──────────────────────────────────╢");
    ArrayMapForeachArray (am => Type[S_ItemType]) {
        server_print("║ %03d │ %-32s ║", __i__, Type[ItemType_Name]);
    }
    server_print("╚═════╧══════════════════════════════════╝");
}