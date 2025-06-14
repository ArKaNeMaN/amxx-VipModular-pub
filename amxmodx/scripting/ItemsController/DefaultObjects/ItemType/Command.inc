#include <amxmodx>
#include <reapi>
#include <ItemsController>

DefaultObjects_ItemType_Command_Register() {
    new T_IC_ItemType:type = IC_ItemType_SimpleRegister(
        .name = "Command",
        .onGive = "@OnCommandGive"
    );
    IC_ItemType_AddParams(type,
        "Command", "String", true,
        "ByServer", "Boolean", false
    );
}

@OnCommandGive(const playerIndex, const Trie:p) {
    static Command[128];
    PCGet_Str(p, "Command", Command, charsmax(Command));

    replace_all(Command, charsmax(Command), "{UserId}", IntToStr(playerIndex));
    replace_all(Command, charsmax(Command), "{playerIndex}", IntToStr(playerIndex));

    if (PCGet_Bool(p, "ByServer", false)) {
        server_cmd(Command);
    } else {
        client_cmd(playerIndex, Command);
    }
}
