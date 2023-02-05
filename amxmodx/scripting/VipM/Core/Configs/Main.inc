#if defined _vipmodular_src_Configs_included
    #endinput
#endif
#define _vipmodular_src_Configs_included

/**
 * Vip Modular: Configs
 */

#include amxmodx
#include json

enum _:S_CfgUnit{
    Array:CfgUnit_LimitUnits, // S_AccessUnit
    Array:CfgUnit_ModuleUnits, // S_ModuleUnit
}

#include "VipM/Core/Configs/Vips"

// Utils

bool:Cfg_ReadParams(
    const JSON:jParams,
    Trie:Params,
    const Array:List = Invalid_Array,
    ErrParam[] = "",
    const Len = 0
) {
    if (!json_is_object(jParams)) {
        return false;
    }

    if (Params == Invalid_Trie) {
        Params = TrieCreate();
    }

    if (List == Invalid_Array) {
        Params = Json_ReadObjectToTrie(jParams, Params);
    } else {
        ArrayForeachArray (List => Param[S_CfgParam]) {
            if (!json_object_has_value(jParams, Param[CfgParam_Name])) {
                if (
                    Param[CfgParam_Required]
                    && Param[CfgParam_Type] != ptCustom
                ) {
                    formatex(ErrParam, Len, Param[CfgParam_Name]);
                    TrieDestroy(Params);
                    return false;
                } else {
                    continue;
                }
            }

            switch (Param[CfgParam_Type]) {
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
