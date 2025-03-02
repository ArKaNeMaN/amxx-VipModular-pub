#include <amxmodx>
#include <reapi>
#include <VipModular>
#include "VipM/Utils"
#include "VipM/DebugMode"
#include "VipM/ArrayTrieUtils"

#pragma semicolon 1
#pragma compress 1

public stock const PluginName[] = "[VipM-M] Spawn Health";
public stock const PluginVersion[] = _VIPM_VERSION;
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = _VIPM_PLUGIN_URL;
public stock const PluginDescription[] = "Vip modular`s module - SpawnHealth";

new const MODULE_NAME[] = "SpawnHealth";

public VipM_OnInitModules(){
    register_plugin(PluginName, PluginVersion, PluginAuthor);

    VipM_Modules_Register(MODULE_NAME, true);
    VipM_Modules_AddParams(MODULE_NAME,
        "Health", ptInteger, false,
        "SetHealth", ptBoolean, false,
        "MaxHealth", ptInteger, false
    );
    VipM_Modules_AddParams(MODULE_NAME,
        "Armor", ptInteger, false,
        "SetArmor", ptBoolean, false,
        "MaxArmor", ptInteger, false
    );
    VipM_Modules_AddParams(MODULE_NAME,
        "Helmet", ptBoolean, false,
        "Limits", ptLimits, false
    );
    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@Event_ModuleActivate");
}

@Event_ModuleActivate(){
    RegisterHookChain(RG_CBasePlayer_Spawn, "@Event_PlayerSpawned", true);
}

@Event_PlayerSpawned(const UserId){
    if (!is_user_alive(UserId)) {
        return;
    }

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    if (Params == Invalid_Trie) {
        return;
    }
    
    Dbg_Log("@Event_PlayerSpawned(%d): Limits Count = %d", UserId, ArraySizeSafe(VipM_Params_GetCell(Params, "Limits", Invalid_Array)));
    if (!VipM_Params_ExecuteLimitsList(Params, "Limits", UserId, Limit_Exec_AND)) {
        return;
    }
    Dbg_Log("@Event_PlayerSpawned(%d): Round №%d -> Passed", UserId, get_member_game(m_iTotalRoundsPlayed) + 1);

    new Health;
    if (TrieGetCell(Params, "Health", Health) && Health > 0) {
        if (!VipM_Params_GetBool(Params, "SetHealth", true)) {
            Health += floatround(get_entvar(UserId, var_health));

            new MaxHealth;
            if (TrieGetCell(Params, "MaxHealth", MaxHealth) && MaxHealth > 0) {
                Health = min(Health, MaxHealth);
            }
        }
        set_entvar(UserId, var_health, float(Health));
    }

    new Armor;
    if (TrieGetCell(Params, "Armor", Armor) && Armor > 0) {
        if (!VipM_Params_GetBool(Params, "SetArmor", true)) {
            Health += rg_get_user_armor(UserId);

            new MaxHealth;
            if (TrieGetCell(Params, "MaxArmor", MaxHealth) && MaxHealth > 0) {
                Health = min(Health, MaxHealth);
            }
        }
        rg_set_user_armor(UserId, Armor, VipM_Params_GetBool(Params, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR);
    }
}
