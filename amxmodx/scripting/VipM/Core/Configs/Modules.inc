
static stock const ALL_MAPS_KEY[] = "_ALL";
static stock const MAPS_CFG_FILE[] = "Maps";

Cfg_LoadModulesConfig(){
    if(!JSON_FILE_EXTSTS(MAPS_CFG_FILE)){
        set_fail_state("[ERROR] Maps config '%s' not found.", GET_FILE_JSON_PATH(MAPS_CFG_FILE));
        return;
    }

    new JSON:Maps = GET_FILE_JSON(MAPS_CFG_FILE);
    if(!json_is_object(Maps)){
        set_fail_state("[ERROR] JSON syntax error in file '%s'.", GET_FILE_JSON_PATH(MAPS_CFG_FILE));
        return;
    }

    new MapName[32];
    get_mapname(MapName, charsmax(MapName));

    new JSON:Map;
    if(json_object_has_value(Maps, MapName, JSONArray))
        Map = json_object_get_value(Maps, MapName);
    else Map = json_object_get_value(Maps, ALL_MAPS_KEY);

    if(!json_is_array(Map)){
        set_fail_state("[ERROR] Invalid maps config format. File '%s'.", GET_FILE_JSON_PATH(MAPS_CFG_FILE));
        return;
    }

    new Module[S_Module], ModuleName[32];

    for(new i = 0; i < json_array_get_count(Map); i++){
        json_array_get_string(Map, i, ModuleName, charsmax(ModuleName));
        if(!MODULE_EXISTS(ModuleName))
            continue;
        
        GET_MODULE(ModuleName, Module);

        new Ret = VIPM_CONTINUE;
        EMIT_MODULE_EVENT(Module, Module_OnActivated, Ret);
        if(Ret == VIPM_STOP)
            continue;
            
        Module[Module_Enabled] = true;
        SET_MODULE(Module);
    }

    json_free(Map);
    json_free(Maps);
}
