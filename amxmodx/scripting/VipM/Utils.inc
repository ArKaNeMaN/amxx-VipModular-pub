#if defined _vipmodular_src_glUtils_included
    #endinput
#endif
#define _vipmodular_src_glUtils_included

#include amxmodx
#include json
#include VipModular

stock const __INT_TEMPLATE_STR[] = "%d";
stock const __SLANG_TEMPLATE_STR[] = "%l";
stock const __CLANG_TEMPLATE_STR[] = "%L";
stock const __JSON_FILE_TEMPLATE_STR[] = "%s.json";
stock const __CMD_LANG_TEMPLATE_STR[] = "%s%d";

#define CompositeMacros(%1) \
    do { %1 } while(is_linux_server() == 0xDEADBEEF)

#define GetRound() \
    get_member_game(m_iTotalRoundsPlayed)+1

#define IntToStr(%1) \
    fmt(__INT_TEMPLATE_STR, %1)

#define IsUserIdValid(%1) \
    (%1 >= 1 && %1 <= 32)

#define IsUserValid(%1) \
    (\
        IsUserIdValid(%1)\
        && is_user_connected(%1)\
    )

#define IsUserValidA(%1) \
    (\
        IsUserIdValid(%1)\
        && is_user_alive(%1)\
    )

#define Lang(%1) \
    fmt(__SLANG_TEMPLATE_STR, %1)

#define ChatPrint(%1,%2) \
    client_print_color(%1, print_team_default, __CLANG_TEMPLATE_STR, IsUserIdValid(%1) ? %1 : -1, "MSG_LAYOUT", fmt(%2))

#define ChatPrintL(%1,%2) \
    ChatPrint(%1, __CLANG_TEMPLATE_STR, %1, %2)

#define GET_FILE(%1) \
    inl_VipM_GetCfgPath(%1)

#define GET_FILE_JSON_PATH(%1) \
    GET_FILE(fmt(__JSON_FILE_TEMPLATE_STR, %1))

#define GET_FILE_JSON(%1) \
    json_parse(GET_FILE_JSON_PATH(%1), true, true)

#define JSON_FILE_EXTSTS(%1) \
    file_exists(GET_FILE_JSON_PATH(%1))

stock Trie:__LangCmdsCache;
stock RegisterClientCommandByLang(const Lang[], const Handler[]){
    TrieCreateIfNotCreated(__LangCmdsCache);

    new Cmd[32];
    new i = 1;
    new langId = LANG_SERVER;
    for(;;){
        if(
            _:GetLangTransKey(fmt(__CMD_LANG_TEMPLATE_STR, Lang, i)) == -1
            || !LookupLangKey(Cmd, charsmax(Cmd), fmt(__CMD_LANG_TEMPLATE_STR, Lang, i), langId)
        ) break;

        register_clcmd(Cmd, Handler);

        if(i == 1)
            TrieSetString(__LangCmdsCache, Lang, Cmd, false);

        i++;
    }
}

stock bool:ClientCmdByLang(const UserId, const Lang[], const fmtParams[] = NULL_STRING, const any:...){
    static Cmd[128];
    if(!GetClientCmdByLang(Cmd, charsmax(Cmd), Lang))
        return false;

    if(fmtParams[0]){
        static Params[256];
        vformat(Params, charsmax(Params), fmtParams, 4);
        add(Cmd, charsmax(Cmd), " ");
        add(Cmd, charsmax(Cmd), Params);
    }
    client_cmd(UserId, Cmd);

    return true;
}

stock bool:GetClientCmdByLang(Out[], const Len, const Lang[]){
    if(TrieGetString(__LangCmdsCache, Lang, Out, Len))
        return true;

    new langId = LANG_SERVER;
    if(
        _:GetLangTransKey(fmt(__CMD_LANG_TEMPLATE_STR, Lang, 1)) == -1
        || !LookupLangKey(Out, Len, fmt(__CMD_LANG_TEMPLATE_STR, Lang, 1), langId)
    ) return false;

    return true;
}

stock __CMD_NULL_ARG = 0;
stock CMD_INIT_PARAMS(){
    __CMD_NULL_ARG = 0;
    static Cmd[32];
    read_argv(__CMD_NULL_ARG, Cmd, charsmax(Cmd));
    if(equali(Cmd, "say", 3))
        __CMD_NULL_ARG++;
}

#define CMD_ARG(%1) \
    (__CMD_NULL_ARG + %1)

#define CMD_ARG_NUM() \
    (read_argc() - __CMD_NULL_ARG - 1)

stock static const JSON_FILE_VALUE_PREFIX[] = "File:";
stock __JSON_LAST_FILE_OBJECT[PLATFORM_MAX_PATH];
stock JSON:JSON_GET_FILE_OR_OBJECT(JSON:Value){
    __JSON_LAST_FILE_OBJECT[0] = 0;

    if(!json_is_string(Value))
        return Value;
    
    new FileName[PLATFORM_MAX_PATH];
    json_get_string(Value, FileName, charsmax(FileName));
    if(!equali(JSON_FILE_VALUE_PREFIX, FileName, charsmax(JSON_FILE_VALUE_PREFIX)))
        return Value;

    json_free(Value);

    formatex(__JSON_LAST_FILE_OBJECT, charsmax(__JSON_LAST_FILE_OBJECT), FileName[charsmax(JSON_FILE_VALUE_PREFIX)]);

    Value = GET_FILE_JSON(__JSON_LAST_FILE_OBJECT);
    if(Value == Invalid_JSON)
        log_amx("[WARNING] JSON syntax error in file `%s`.", __JSON_LAST_FILE_OBJECT);

    return Value;
}

#define GetLastJsonObjectFile() \
    __JSON_LAST_FILE_OBJECT

#define HasLastJsonObjectFile() \
    bool:(GetLastJsonObjectFile()[0])

#define SaveLastJsonObjectFile(%1) \
    new %1[PLATFORM_MAX_PATH];copy(%1, charsmax(%1), GetLastJsonObjectFile())

stock bool:Json_IsRef(const JSON:jValue, FileName[] = NULL_STRING, const Len = 0){
    if(!json_is_string(jValue))
        return false;

    new Str[128];
    json_get_string(jValue, Str, charsmax(Str));
    if(!equali(JSON_FILE_VALUE_PREFIX, Str, charsmax(JSON_FILE_VALUE_PREFIX)))
        return false;

    if(Len > 0)
        copy(FileName, Len, Str[charsmax(JSON_FILE_VALUE_PREFIX)]);
    
    return true;
}


#define json_array_foreach(%1:%2) \
    if(json_is_array(%1)) \
        for(new %2 = 0; %2 < json_array_get_count(%1); %2++)

#define json_array_foreach_vars(%1:%2,[%3]) \
    if(json_is_array(%1)) \
        for(new %3, %2 = 0; %2 < json_array_get_count(%1); %2++)

#define json_array_foreach_value(%1:%2=>%3) \
    json_array_foreach_vars(%1: %2, [JSON:%3]) \
        if((%3 = json_array_get_value(%1, %2)) != Invalid_JSON)

#define json_get_count(%1) \
    (json_is_array(%1) ? json_array_get_count(%1) : 1)


#define BitSet(%1,%2) \
    %1 |= (1 << (%2 - 1))

#define BitReset(%1,%2) \
    %1 &= ~(1 << (%2 - 1))

#define BitIs(%1,%2) \
    (bool:(%1 & (1 << (%2 - 1))))
