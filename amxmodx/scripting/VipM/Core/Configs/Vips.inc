#include amxmodx
#include json
#include regex

static stock const VIPS_CFG_FILE[] = "Vips";
static stock const VIPS_CFGS_DIR[] = "Vips";

Array:Cfg_LoadVipsConfigs(Array:aList = Invalid_Array) {
    if (JSON_FILE_EXTSTS(VIPS_CFG_FILE)) {
        aList = Cfg_LoadVipsConfigFromFile(VIPS_CFG_FILE, aList);
    }

    new sFilePath[PLATFORM_MAX_PATH], iDirHandler, FileType:Type;
    iDirHandler = open_dir(GET_FILE(VIPS_CFGS_DIR), sFilePath, charsmax(sFilePath), Type);
    if (!iDirHandler) {
        return aList;
    }

    new ret, Regex:RegEx_FileName = regex_compile("(.+).json$", ret, "", 0, "i");
    do {
        if (
            sFilePath[0] == '!'
            || Type != FileType_File
            || regex_match_c(sFilePath, RegEx_FileName) <= 0
        ) {
            continue;
        }

        new sFileName[64];
        regex_substr(RegEx_FileName, 1, sFileName, charsmax(sFileName));

        aList = Cfg_LoadVipsConfigFromFile(fmt("%s/%s", VIPS_CFGS_DIR, sFileName), aList);
    } while (next_file(iDirHandler, sFilePath, charsmax(sFilePath), Type));
    
    return aList;
}

Array:Cfg_LoadVipsConfigFromFile(const sFileName[], Array:aList = Invalid_Array) {
    if (!JSON_FILE_EXTSTS(sFileName)) {
        set_fail_state("[ERROR] Vips config '%s' not found.", GET_FILE_JSON_PATH(sFileName));
        return Invalid_Array;
    }

    new JSON:jVips = GET_FILE_JSON(sFileName);
    if (!json_is_array(jVips)) {
        set_fail_state("[ERROR] JSON syntax error in file '%s'.", GET_FILE_JSON_PATH(sFileName));
        return Invalid_Array;
    }

    ArrayCreateIfNotCreated(aList, S_CfgUnit, json_array_get_count(jVips));
    new CfgUnit[S_CfgUnit];
    json_array_foreach_value (jVips: i => jCfgUnit) {
        if (Cfg_ReadVipConfig(jCfgUnit, CfgUnit)) {
            ArrayPushArray(aList, CfgUnit);
        }
        json_free(jCfgUnit);
    }
    
    Json_DeepFree(jVips);
    return aList;
}

bool:Cfg_ReadVipConfig(const JSON:jCfgUnit, CfgUnit[S_CfgUnit]) {
    new JSON:jAccess = json_object_get_value(jCfgUnit, "Access");
    if (jAccess == Invalid_JSON) {
        return false;
    }

    CfgUnit[CfgUnit_LimitUnits] = Limits_LoadUnitListFromJson(jAccess);
    json_free(jAccess);
    if (!ArraySizeSafe(CfgUnit[CfgUnit_LimitUnits])) {
        ArrayDestroy(CfgUnit[CfgUnit_LimitUnits]);
        return false;
    }
        
    new JSON:jModules = json_object_get_value(jCfgUnit, "Modules");
    if (jModules == Invalid_JSON) {
        ArrayDestroy(CfgUnit[CfgUnit_LimitUnits]);
        return false;
    }

    CfgUnit[CfgUnit_ModuleUnits] = ModuleUnits_LoadListFromJson(jModules);
    json_free(jModules);
    if (!ArraySizeSafe(CfgUnit[CfgUnit_ModuleUnits])) {
        // TODO: Тут нужен специальный дестрой для ограничений, иначе утечка говна)
        // Но эта штука случается только при кривых конфигах, так что не критично)))
        ArrayDestroy(CfgUnit[CfgUnit_LimitUnits]);
        ArrayDestroy(CfgUnit[CfgUnit_ModuleUnits]);
        return false;
    }

    return true;
}
