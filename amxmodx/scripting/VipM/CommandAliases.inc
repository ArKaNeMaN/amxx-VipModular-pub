#if defined _command_aliases_included
    #endinput
#endif
#define _command_aliases_included

#include <amxmodx>
#include <json>
// #include "VipM/DebugMode"

#define COMMAND_ALIASES_COMMAND_MAX_LENGTH 64
static const DEFAULT_TEMPLATE_PLACEHOLDER[] = "<cmd>";
static const DEFAULT_TEMPLATES[][] = {
    "say /<cmd>",
    "say_team /<cmd>",
};

enum _:{
    CommandAliases_CmdType_Client = (1 << 0),
    CommandAliases_CmdType_Console = (1 << 1),
    CommandAliases_CmdType_Server = (1 << 2),
}

static JSON:jFile = Invalid_JSON;
static Trie:g_tMainCmd = Invalid_Trie;

stock bool:CommandAliases_Open(const sFilePath[], const bool:bCreateIfNotExists = false) {
    jFile = Invalid_JSON;
    
    if (!file_exists(sFilePath)) {
        if (bCreateIfNotExists) {
            new JSON:jObj = json_init_object();
            json_serial_to_file(jObj, sFilePath, true);
            json_free(jObj);
        } else {
            return false;
        }
    }

    jFile = json_parse(sFilePath, true, true);

    if (jFile == Invalid_JSON) {
        log_amx("[CommandAliases][Warn] Can't read from file: %s", sFilePath);
        return false;
    }

    if (!json_is_object(jFile)) {
        jFile = Invalid_JSON;
        log_amx("[CommandAliases][Warn] Invalid file format: %s", sFilePath);
        return false;
    }

    return true;
}

stock CommandAliases_Close() {
    json_free(jFile);
}

static stock Array:ReadCommandsByKey(const sCmdKey[]) {
    new Array:aCmds = Invalid_Array;

    if (jFile == Invalid_JSON) {
        aCmds = ArrayCreate(COMMAND_ALIASES_COMMAND_MAX_LENGTH, sizeof DEFAULT_TEMPLATES);
        ArrayPushString(aCmds, sCmdKey);

        for (new i = 0; i < sizeof DEFAULT_TEMPLATES; i++) {
            new sCmd[COMMAND_ALIASES_COMMAND_MAX_LENGTH];
            copy(sCmd, charsmax(sCmd), DEFAULT_TEMPLATES[i]);
            replace(sCmd, charsmax(sCmd), DEFAULT_TEMPLATE_PLACEHOLDER, sCmdKey);
            ArrayPushString(aCmds, sCmd);
        }
    } else {
        new JSON:jCmds = json_object_get_value(jFile, sCmdKey);
        if (!json_is_array(jCmds)) {
            log_amx("[CommandAliases][Warn] Invalid file format. Commands list must be an array.");
            return aCmds;
        }

        aCmds = ArrayCreate(COMMAND_ALIASES_COMMAND_MAX_LENGTH, json_array_get_count(jCmds));

        for (new i = 0; i < json_array_get_count(jCmds); i++) {
            new sCmd[COMMAND_ALIASES_COMMAND_MAX_LENGTH];
            json_array_get_string(jCmds, i, sCmd, charsmax(sCmd));
            ArrayPushString(aCmds, sCmd);
        }
        json_free(jCmds);
    }

    return aCmds;
}

stock CommandAliases_Register(const sCmdKey[], const sCallback[], const bitCmdTypes) {
    new Array:aCmds = ReadCommandsByKey(sCmdKey);
    if (aCmds == Invalid_Array || !ArraySize(aCmds)) {
        return;
    }

    if (g_tMainCmd == Invalid_Trie) {
        g_tMainCmd = TrieCreate();
    }

    new sCmd[COMMAND_ALIASES_COMMAND_MAX_LENGTH];
    ArrayGetString(aCmds, 0, sCmd, charsmax(sCmd));
    TrieSetString(g_tMainCmd, sCmdKey, sCmd);

    for (new i = 0; i < ArraySize(aCmds); i++) {
        ArrayGetString(aCmds, i, sCmd, charsmax(sCmd));
        
        if (bitCmdTypes & CommandAliases_CmdType_Client) {
            register_clcmd(sCmd, sCallback);
            // Dbg_PrintServer("[CommandAliases][Register] %s: Client - %s [%s]", sCmdKey, sCmd, sCallback);
        }
        
        if (bitCmdTypes & CommandAliases_CmdType_Console) {
            register_concmd(sCmd, sCallback);
            // Dbg_PrintServer("[CommandAliases][Register] %s: Console - %s [%s]", sCmdKey, sCmd, sCallback);
        }
        
        if (bitCmdTypes & CommandAliases_CmdType_Server) {
            register_srvcmd(sCmd, sCallback);
            // Dbg_PrintServer("[CommandAliases][Register] %s: Server - %s [%s]", sCmdKey, sCmd, sCallback);
        }
    }
    ArrayDestroy(aCmds);
}

stock CommandAliases_RegisterClient(const sCmdKey[], const sCallback[]) {
    CommandAliases_Register(sCmdKey, sCallback, CommandAliases_CmdType_Client);
}

stock CommandAliases_RegisterConsole(const sCmdKey[], const sCallback[]) {
    CommandAliases_Register(sCmdKey, sCallback, CommandAliases_CmdType_Console);
}

stock CommandAliases_RegisterServer(const sCmdKey[], const sCallback[]) {
    CommandAliases_Register(sCmdKey, sCallback, CommandAliases_CmdType_Server);
}

stock CommandAliases_RegisterAll(const sCmdKey[], const sCallback[]) {
    CommandAliases_Register(sCmdKey, sCallback, CommandAliases_CmdType_Client|CommandAliases_CmdType_Console|CommandAliases_CmdType_Server);
}

stock bool:CommandAliases_GetMainCmd(const sCmdKey[], sOut[], const iOutLen) {
    return TrieGetString(g_tMainCmd, sCmdKey, sOut, iOutLen);
}

stock bool:CommandAliases_ClientCmd(const UserId, const sCmdKey[], const sArgsFmt[] = "", any:...) {
    static sCmd[COMMAND_ALIASES_COMMAND_MAX_LENGTH], sFormattedArgs[256];
    if (CommandAliases_GetMainCmd(sCmdKey, sCmd, charsmax(sCmd))) {
        if (sArgsFmt[0]) {
            sFormattedArgs[0] = 0;
            vformat(sFormattedArgs, charsmax(sFormattedArgs), sArgsFmt, 4);
            client_cmd(UserId, "%s %s", sCmd, sFormattedArgs);
        } else {
            client_cmd(UserId, "%s", sCmd);
        }
    }
}
