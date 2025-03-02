#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>
#include <VipModular> // Для VipM_Params_* // TODO: уйти на аналог от контроллера параметров, когда он будет

static HookChain:PlayerSpawnPreHookChain = INVALID_HOOKCHAIN;
static HookChain:PlayerTakeDamagePreHookChain = INVALID_HOOKCHAIN;

static Float:PlayerGivenDmgMult[MAX_PLAYERS + 1] = {1.0, ...};
static Float:PlayerTakenDmgMult[MAX_PLAYERS + 1] = {1.0, ...};

DefaultObjects_ItemType_DamageMult_Register() {
    IC_ItemType_SimpleRegister(
        .name = "DamageMult",
        .onRead = "@ItemType_DamageMult_OnRead",
        .onGive = "@ItemType_DamageMult_OnGive"
    );
    
    DisableHookChain(PlayerSpawnPreHookChain = RegisterHookChain(RG_CBasePlayer_Spawn, "@ItemType_DamageMult_OnPlayerSpawnPre", false));
    DisableHookChain(PlayerTakeDamagePreHookChain = RegisterHookChain(RG_CBasePlayer_TakeDamage, "@ItemType_DamageMult_OnPlayerTakeDamage", false));
}

@ItemType_DamageMult_OnRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (json_object_has_value(instanceJson, "Given", JSONNumber)) {
        TrieSetCell(p, "Given", json_object_get_real(instanceJson, "Given"));
    }

    if (json_object_has_value(instanceJson, "Taken", JSONNumber)) {
        TrieSetCell(p, "Taken", json_object_get_real(instanceJson, "Taken"));
    }
    
    EnableHookChain(PlayerSpawnPreHookChain);
    EnableHookChain(PlayerTakeDamagePreHookChain);

    return IC_RET_READ_SUCCESS;
}

@ItemType_DamageMult_OnGive(const playerIndex, const Trie:p) {
    PlayerGivenDmgMult[playerIndex] = VipM_Params_GetFloat(p, "Given", 1.0);
    PlayerTakenDmgMult[playerIndex] = VipM_Params_GetFloat(p, "Taken", 1.0);
}

@ItemType_DamageMult_OnPlayerSpawnPre(const playerIndex) {
    PlayerGivenDmgMult[playerIndex] = 1.0;
    PlayerTakenDmgMult[playerIndex] = 1.0;
}

@ItemType_DamageMult_OnPlayerTakeDamage(const victimIndex, inflictorIndex, attackerInde, Float:damage, damageType) {
    if (is_user_connected(attackerInde)) {
        damage *= PlayerGivenDmgMult[attackerInde];
    }

    if (is_user_connected(victimIndex)) {
        damage *= PlayerTakenDmgMult[victimIndex];
    }

    SetHookChainArg(4, ATYPE_FLOAT, damage);
    return HC_CONTINUE;
}
