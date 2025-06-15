#include <amxmodx>
#include <json>
#include <ParamsController>
#include <VipModular>

DefaultObjects_ParamType_Limits_Register() {
    ParamsController_RegSimpleType(VIPM_PARAM_TYPE_LIMITS_NAME, "@OnLimitsParamTypeRead");
}

@OnLimitsParamTypeRead(const JSON:valueJson) {
    new Array:limits = VipM_Limits_ReadListFromJson(valueJson);

    if (limits == Invalid_Array) {
        return false;
    }

    if (
        (
            json_is_object(valueJson)
            || (json_is_array(valueJson) && json_array_get_count(valueJson) == 0)
        ) && ArraySize(limits) < 1
    ) {
        PCJson_LogForFile(valueJson, "WARNING", "All limits are invalid.");
        return false;
    }

    return ParamsController_SetCell(limits);
}
