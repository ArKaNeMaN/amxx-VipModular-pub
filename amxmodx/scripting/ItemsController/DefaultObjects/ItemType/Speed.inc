#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>
#include <VipModular> // Для VipM_Params_* // TODO: уйти на аналог от контроллера параметров, когда он будет

static HookChain:PlayerSpawnPreHookChain = INVALID_HOOKCHAIN;
static HookChain:PlayerResetSpeedPostHookChain = INVALID_HOOKCHAIN;

static Float:PlayerSpeedMult[MAX_PLAYERS + 1] = {1.0, ...};

DefaultObjects_ItemType_Speed_Register() {
    IC_ItemType_SimpleRegister(
        .name = "Speed",
        .onRead = "@ItemType_Speed_OnRead",
        .onGive = "@ItemType_Speed_OnGive"
    );
    
    DisableHookChain(PlayerResetSpeedPostHookChain = RegisterHookChain(RG_CBasePlayer_ResetMaxSpeed, "@ItemType_Speed_OnPlayerResetSpeedPost", true));
    DisableHookChain(PlayerSpawnPreHookChain = RegisterHookChain(RG_CBasePlayer_Spawn, "@ItemType_Speed_OnPlayerSpawnPre", false));
}

@ItemType_Speed_OnRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Multiplier", JSONNumber)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Multiplier` required for item `Speed`.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Multiplier", json_object_get_real(instanceJson, "Multiplier"));

    EnableHookChain(PlayerResetSpeedPostHookChain);
    EnableHookChain(PlayerSpawnPreHookChain);

    return IC_RET_READ_SUCCESS;
}

@ItemType_Speed_OnGive(const playerIndex, const Trie:p) {
    PlayerSpeedMult[playerIndex] = VipM_Params_GetFloat(p, "Multiplier", 1.0);
    ItemType_Speed_MultUserSpeed(playerIndex, PlayerSpeedMult[playerIndex]);
}

@ItemType_Speed_OnPlayerSpawnPre(const playerIndex) {
    PlayerSpeedMult[playerIndex] = 1.0;
}

@ItemType_Speed_OnPlayerResetSpeedPost(const playerIndex) {
    ItemType_Speed_MultUserSpeed(playerIndex, PlayerSpeedMult[playerIndex]);
}

static ItemType_Speed_MultUserSpeed(const playerIndex, const Float:mult) {
    if (mult != 1.0) {
        set_entvar(playerIndex, var_maxspeed, Float:get_entvar(playerIndex, var_maxspeed) * mult);
    }
}
