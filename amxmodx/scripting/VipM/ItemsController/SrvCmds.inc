SrvCmds_Init(){
    register_srvcmd("vipm_ic_types", "@SrvCmd_Types");
}

@SrvCmd_Types(){
    PrintTypesData(Types);
}