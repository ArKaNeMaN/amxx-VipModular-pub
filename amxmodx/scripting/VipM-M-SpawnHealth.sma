#include <amxmodx>
#include <reapi>
#include <VipModular>

#pragma semicolon 1
#pragma compress 1

#define GetRound() \
    get_member_game(m_iTotalRoundsPlayed)+1

public stock const PluginName[] = "[VipM][M] Spawn Health";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginURL[] = "https://arkanaplugins.ru/plugin/9";
public stock const PluginDescription[] = "Vip modular`s module - SpawnHealth";

new const MODULE_NAME[] = "SpawnHealth";

public VipM_OnInitModules(){
    register_plugin(PluginName, VIPM_VERSION, PluginAuthor);

    VipM_Modules_Register(MODULE_NAME, true);

    VipM_Modules_RegisterEvent(MODULE_NAME, Module_OnActivated, "@Event_ModuleActivate");
}

@Event_ModuleActivate(){
    RegisterHookChain(RG_CBasePlayer_Spawn, "@Event_PlayerSpawned", true);
}

@Event_PlayerSpawned(const UserId){
    if(!is_user_alive(UserId))
        return;

    new Trie:Params = VipM_Modules_GetParams(MODULE_NAME, UserId);
    if(Params == Invalid_Trie)
        return;

    // TODO: Заменить MinRound на лимиты
    if(GetRound() < VipM_Params_GetInt(Params, "MinRound", 0))
        return;

    new Health;
    if(TrieGetCell(Params, "Health", Health) && Health > 0){
        if(!VipM_Params_GetBool(Params, "SetHealth", true)){
            Health += floatround(get_entvar(UserId, var_health));

            new MaxHealth;
            if(TrieGetCell(Params, "MaxHealth", MaxHealth) && MaxHealth > 0)
                Health = min(Health, MaxHealth);
        }
        set_entvar(UserId, var_health, float(Health));
    }

    new Armor;
    if(TrieGetCell(Params, "Armor", Armor) && Armor > 0){
        if(!VipM_Params_GetBool(Params, "SetArmor", true)){
            Health += rg_get_user_armor(UserId);

            new MaxHealth;
            if(TrieGetCell(Params, "MaxArmor", MaxHealth) && MaxHealth > 0)
                Health = min(Health, MaxHealth);
        }
        rg_set_user_armor(UserId, Armor, VipM_Params_GetBool(Params, "Helmet", false) ? ARMOR_VESTHELM : ARMOR_KEVLAR);
    }
}
