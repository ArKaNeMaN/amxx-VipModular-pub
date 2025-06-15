#include <ItemsController>

#include "VipM/DefaultObjects/ParamType/Limit"
#include "VipM/DefaultObjects/ParamType/Limits"

public ParamsController_OnRegisterTypes() {
    DefaultObjects_ParamType_Limit_Register();
    DefaultObjects_ParamType_Limits_Register();
}