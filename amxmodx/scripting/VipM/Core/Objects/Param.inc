#if defined __vipm_core_objects_param_included
    #endinput
#endif
#define __vipm_core_objects_param_included

#include <amxmodx>
#include <json>
#include <VipM/Params>
#include <VipM/Limits>
#include "VipM/ArrayTrieUtils"

enum _:S_CfgParam {
    CfgParam_Name[VIPM_PARAM_NAME_MAX_LEN],
    E_ParamType:CfgParam_Type,
    bool:CfgParam_Required,
}

Array:CfgParam_GetFromNative(const iStartParam, const iNativeParamsCount, &Array:aParams = Invalid_Array) {
    const ARGS_NUM_PER_PARAM = 3;

    new NullArg = iStartParam - 1;

    if ((iNativeParamsCount - NullArg) % ARGS_NUM_PER_PARAM != 0) {
        log_error(0, "[ERROR] Invalid parameters num.");
        return Invalid_Array;
    }

    new iParamsCount = max(1, (iNativeParamsCount - NullArg) / ARGS_NUM_PER_PARAM);
    if (aParams == Invalid_Array) {
        aParams = ArrayCreate(S_CfgParam, iParamsCount);
    } else {
        ArrayResize(aParams, ArraySize(aParams) + iParamsCount);
    }

    new Param[S_CfgParam];
    for (new i = NullArg + 1; i < iNativeParamsCount; i += ARGS_NUM_PER_PARAM) {
        get_string(i, Param[CfgParam_Name], charsmax(Param[CfgParam_Name]));
        Param[CfgParam_Type] = E_ParamType:get_param_byref(i + 1);
        Param[CfgParam_Required] = bool:get_param_byref(i + 2);

        ArrayPushArray(aParams, Param);
    }

    return aParams;
}

Array:CfgParam_Merge(&Array:aParams, const Array:...) {
    if (aParams == Invalid_Array) {
        aParams = ArrayCreate(S_CfgParam, 1);
    }

    for (new i = 1, ii = numargs(); i < ii; ++i) {
        new Array:aMerge = Array:getarg(i);

        for (new j = 0, jj = ArraySizeSafe(aMerge); j < jj; ++j) {
            new Param[S_CfgParam];
            ArrayGetArray(aMerge, j, Param);

            ArrayPushArray(aParams, Param);
        }
    }

    return aParams;
}

bool:CfgParam_ReadList(
    const JSON:jParams,
    &Trie:Params,
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
                    return false;
                } else {
                    continue;
                }
            }

            switch (Param[CfgParam_Type]) {
                case ptCustom:
                    continue;

                case ptInteger:
                    TrieSetCell(Params, Param[CfgParam_Name], Json_Object_GetInt(jParams, Param[CfgParam_Name]));
                    
                case ptFloat:
                    TrieSetCell(Params, Param[CfgParam_Name], Json_Object_GetFloat(jParams, Param[CfgParam_Name]));
                    
                case ptBoolean:
                    TrieSetCell(Params, Param[CfgParam_Name], Json_Object_GetBool(jParams, Param[CfgParam_Name]));

                case ptString: {
                    new sParam[128];
                    Json_Object_GetString(jParams, Param[CfgParam_Name], sParam, charsmax(sParam));
                    TrieSetString(Params, Param[CfgParam_Name], sParam);
                }

                case ptColor: {
                    new JSON:jColor = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Color[3];
                    for (new i = 0; i < 3; ++i) {
                        Color[i] = json_array_get_number(jColor, i);
                    }
                    TrieSetArray(Params, Param[CfgParam_Name], Color, 3);
                    json_free(jColor);
                }

                case ptVector2: {
                    new JSON:jVec = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Float:Vec[2];
                    for (new i = 0; i < 2; ++i) {
                        Vec[i] = json_array_get_real(jVec, i);
                    }
                    TrieSetArray(Params, Param[CfgParam_Name], Vec, 2);
                    json_free(jVec);
                }
                case ptVector3: {
                    new JSON:jVec = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Float:Vec[3];
                    for (new i = 0; i < 3; ++i) {
                        Vec[i] = json_array_get_real(jVec, i);
                    }
                    TrieSetArray(Params, Param[CfgParam_Name], Vec, 3);
                    json_free(jVec);
                }

                case ptLimit: {
                    new JSON:jLimit = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new T_LimitUnit:iLimit = VipM_Limits_ReadFromJson(jLimit);
                    TrieSetCell(Params, Param[CfgParam_Name], iLimit);
                    json_free(jLimit);
                }
                case ptLimits: {
                    new JSON:jLimits = json_object_get_value(jParams, Param[CfgParam_Name]);
                    new Array:aLimits = VipM_Limits_ReadListFromJson(jLimits, aLimits);
                    TrieSetCell(Params, Param[CfgParam_Name], aLimits);
                    json_free(jLimits);
                }
            }
        }
    }

    return true;
}
