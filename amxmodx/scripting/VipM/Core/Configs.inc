#if defined _vipmodular_src_Configs_included
    #endinput
#endif
#define _vipmodular_src_Configs_included

/**
 * Vip Modular: Configs
 */

#include amxmodx
#include json

static stock const ALL_MAPS_KEY[] = "_ALL";
static stock const MAPS_CFG_FILE[] = "Maps";
static stock const VIPS_CFG_FILE[] = "Vips";

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

// Vips

Array:Cfg_ReadVipConfigs(){
    if(!JSON_FILE_EXTSTS(VIPS_CFG_FILE)){
        set_fail_state("[ERROR] Main config '%s' not found.", GET_FILE_JSON_PATH(VIPS_CFG_FILE));
        return Invalid_Array;
    }

    new JSON:jVips = GET_FILE_JSON(VIPS_CFG_FILE);
    if(!json_is_array(jVips)){
        set_fail_state("[ERROR] JSON syntax error in file '%s'.", GET_FILE_JSON_PATH(VIPS_CFG_FILE));
        return Invalid_Array;
    }

    new Array:List = ArrayCreate(S_CfgUnit, json_array_get_count(jVips));
    new CfgUnit[S_CfgUnit];
    for(new i = 0; i < json_array_get_count(jVips); i++){
        new JSON:jCfgUnit = json_array_get_value(jVips, i);
        
        new JSON:jAccess = json_object_get_value(jCfgUnit, "Access");
        jAccess = JSON_GET_FILE_OR_OBJECT(jAccess); // TODO: Use Json_IsRef instead JSON_GET_FILE_OR_OBJECT
        if(jAccess == Invalid_JSON)
            continue;

        CfgUnit[CfgUnit_LimitUnits] = Limits_LoadUnitListFromJson(jAccess);
        json_free(jAccess);
        if(CfgUnit[CfgUnit_LimitUnits] == Invalid_Array)
            continue;
            
        new JSON:jModules = json_object_get_value(jCfgUnit, "Modules");
        jModules = JSON_GET_FILE_OR_OBJECT(jModules);
        if(jModules == Invalid_JSON)
            continue;

        CfgUnit[CfgUnit_ModuleUnits] = ModuleUnits_LoadListFromJson(jModules);
        json_free(jModules);
        if(CfgUnit[CfgUnit_ModuleUnits] == Invalid_Array)
            continue;

        ArrayPushArray(List, CfgUnit);
        json_free(jCfgUnit);
    }
    
    json_free(jVips);
    return List;
}

// Utils

bool:Cfg_ReadParams(const JSON:jParams, Trie:Params, const Array:List = Invalid_Array, ErrParam[] = "", const Len = 0){
    if(!json_is_object(jParams))
        return false;

    if(Params == Invalid_Trie)
        Params = TrieCreate();

    if(List == Invalid_Array){
        new ParamName[32];
        for(new i = 0; i < json_object_get_count(jParams); i++){

            json_object_get_name(jParams, i, ParamName, charsmax(ParamName));
            new JSON:jParam = json_object_get_value_at(jParams, i);

            switch(json_get_type(jParam)){
                case JSONString: {
                    new sParam[128];
                    json_get_string(jParam, sParam, charsmax(sParam));
                    TrieSetString(Params, ParamName, sParam);
                }
                
                case JSONNumber:
                    TrieSetCell(Params, ParamName, json_get_number(jParam));
                    
                case JSONBoolean:
                    TrieSetCell(Params, ParamName, json_get_bool(jParam));
                    
            }
            json_free(jParam);
        }
    }
    else{
        ArrayForeachArray(List => Param[S_CfgParam]){
            if(!json_object_has_value(jParams, Param[CfgParam_Name])){
                if(
                    Param[CfgParam_Required]
                    && Param[CfgParam_Type] != ptCustom
                ){
                    formatex(ErrParam, Len, Param[CfgParam_Name]);
                    TrieDestroy(Params);
                    return false;
                }
                else continue;
            }

            switch(Param[CfgParam_Type]){
                case ptCustom:
                    continue;

                case ptInteger:
                    TrieSetCell(Params, Param[CfgParam_Name], json_object_get_number(jParams, Param[CfgParam_Name]));
                    
                case ptFloat:
                    TrieSetCell(Params, Param[CfgParam_Name], json_object_get_real(jParams, Param[CfgParam_Name]));
                    
                case ptBoolean:
                    TrieSetCell(Params, Param[CfgParam_Name], json_object_get_bool(jParams, Param[CfgParam_Name]));

                case ptString: {
                    new sParam[128];
                    json_object_get_string(jParams, Param[CfgParam_Name], sParam, charsmax(sParam));
                    TrieSetString(Params, Param[CfgParam_Name], sParam);
                }

                case ptColor: {
                    new JSON:jColor = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Color[3];
                    for(new i = 0; i < 3; i++)
                        Color[i] = json_array_get_number(jColor, i);
                    TrieSetArray(Params, Param[CfgParam_Name], Color, 3);
                    json_free(jColor);
                }

                case ptVector2: {
                    new JSON:jVec = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Float:Vec[2];
                    for(new i = 0; i < 2; i++)
                        Vec[i] = json_array_get_real(jVec, i);
                    TrieSetArray(Params, Param[CfgParam_Name], Vec, 2);
                    json_free(jVec);
                }

                case ptVector3: {
                    new JSON:jVec = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Float:Vec[3];
                    for(new i = 0; i < 3; i++)
                        Vec[i] = json_array_get_real(jVec, i);
                    TrieSetArray(Params, Param[CfgParam_Name], Vec, 3);
                    json_free(jVec);
                }

                case ptLimit: {
                    new JSON:jLimit = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new T_LimitUnit:iLimit = Limits_LoadUnitFromJson(jLimit);
                    TrieSetCell(Params, Param[CfgParam_Name], iLimit);
                    json_free(jLimit);
                }
                case ptLimits: {
                    new JSON:jLimits = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Array:aLimits = Limits_LoadUnitListFromJson(jLimits, aLimits);
                    TrieSetCell(Params, Param[CfgParam_Name], aLimits);
                    json_free(jLimits);
                }
            }
        }
    }

    return true;
}