#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_Command_Register() {
    IC_ItemType_SimpleRegister(
        .name = "Command",
        .onRead = "@OnCommandRead",
        .onGive = "@OnCommandGive"
    );
}

@OnCommandRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Command", JSONString)) {
        Json_LogForFile(instanceJson, "ERROR", "Param `Command` required for item `Command`.");
        return IC_RET_READ_FAIL;
    }
    
    new Command[128];
    json_object_get_string(instanceJson, "Command", Command, charsmax(Command));
    TrieSetString(p, "Command", Command);

    if (json_object_has_value(instanceJson, "ByServer", JSONBoolean)) {
        TrieSetCell(p, "ByServer", json_object_get_bool(instanceJson, "ByServer"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnCommandGive(const playerIndex, const Trie:p) {
    static Command[128];
    VipM_Params_GetStr(p, "Command", Command, charsmax(Command));
    new bool:ByServer = VipM_Params_GetBool(p, "ByServer", false);

    replace_all(Command, charsmax(Command), "{playerIndex}", IntToStr(playerIndex));

    if (ByServer) {
        server_cmd(Command);
    } else {
        client_cmd(playerIndex, Command);
    }
}
