#include <amxmodx>
#include <reapi515>
#include <VipModular>

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM][M] Vampire";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Vip modular`s module - Vampire";

new const MODULE_NAME[] = "Vampire";

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);
    register_dictionary("VipM-Vampire.ini");

    VipM_Modules_Register(MODULE_NAME, true);
    VipM_SetModuleParams(MODULE_NAME,
        "MaxHealth", ptInteger, false,
        "ByKill", ptInteger, false,
        "ByHead", ptInteger, false,
        "ByKnife", ptInteger, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@Event_ModuleActivate");
}

@Event_ModuleActivate(){
    RegisterHookChain(RG_CBasePlayer_Killed, "@Event_PlayerKilled", true);
}

@Event_PlayerKilled(const VictimId, UserId, InflictorId){
    if(
        UserId == VictimId
        || !is_user_alive(UserId)
        || !is_user_connected(VictimId)
    ) return;

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    if(Params == Invalid_Trie)
        return;

    new MaxHealth = VipM_Params_GetInt(Params, "MaxHealth", 100);
    new Health = floatround(get_entvar(UserId, var_health));
    if(Health >= MaxHealth)
        return;
    
    new ByKill = VipM_Params_GetInt(Params, "ByKill", 0);
    new VampHealth = 0;
    new ActiveItem; ActiveItem = get_member(UserId, m_pActiveItem);
    if(
        !(get_member(VictimId, m_bitsDamageType) & DMG_SLASH)
        && is_entity(ActiveItem)
        && rg_get_iteminfo(ActiveItem, ItemInfo_iId) == CSW_KNIFE
    )
        VampHealth = VipM_Params_GetInt(Params, "ByKnife", ByKill);
    else if(get_member(VictimId, m_bHeadshotKilled))
        VampHealth = VipM_Params_GetInt(Params, "ByHead", ByKill);
    else VampHealth = ByKill;
    

    client_print(UserId, print_center, "%L", UserId, "VAMPIRE_HEALTH_MESSAGE", VampHealth);
    Health = clamp(Health + VampHealth, 1, MaxHealth < 1 ? cellmax : MaxHealth);
    set_entvar(UserId, var_health, float(Health));
}
