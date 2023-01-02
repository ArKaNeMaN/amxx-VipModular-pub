#if defined _vipmodular_src_glUtils_included
    #endinput
#endif
#define _vipmodular_src_glUtils_included

// Надо наверное как-то их обьединить)
#include "VipM/ArrayTrieUtils"

#include amxmodx
#include json
#include reapi
#include VipModular

stock const __INT_TEMPLATE_STR[] = "%d";
stock const __SLANG_TEMPLATE_STR[] = "%l";
stock const __CLANG_TEMPLATE_STR[] = "%L";
stock const __JSON_FILE_TEMPLATE_STR[] = "%s.json";
stock const __USER_NAME_TEMPLATE_STR[] = "%n";

#define CompositeMacros(%1) \
    do { %1 } while(is_linux_server() == 0xDEADBEEF)

#define GetRound() \
    get_member_game(m_iTotalRoundsPlayed)+1

#define IntToStr(%1) \
    fmt(__INT_TEMPLATE_STR, %1)

#define IsUserIdValid(%1) \
    (%1 >= 1 && %1 <= 32)

#define IsUserValid(%1) \
    ( \
        IsUserIdValid(%1) \
        && is_user_connected(%1) \
    )

#define IsUserValidA(%1) \
    ( \
        IsUserIdValid(%1) \
        && is_user_alive(%1) \
    )

#define Lang(%1) \
    fmt(__SLANG_TEMPLATE_STR, %1)

#define uLang(%1) \
    fmt(__CLANG_TEMPLATE_STR, %1)

#define ChatPrint(%1,%2) \
    client_print_color(%1, print_team_default, __CLANG_TEMPLATE_STR, IsUserIdValid(%1) ? %1 : -1, "MSG_LAYOUT", fmt(%2))

#define ChatPrintL(%1,%2) \
    ChatPrint(%1, __CLANG_TEMPLATE_STR, %1, %2)

#define GET_FILE(%1) \
    inl_VipM_GetCfgPath(%1)

#define GET_FILE_JSON_PATH(%1) \
    GET_FILE(fmt(__JSON_FILE_TEMPLATE_STR, %1))

#define GET_FILE_JSON(%1) \
    Json_ParseFromFileEx(GET_FILE_JSON_PATH(%1))

#define JSON_FILE_EXTSTS(%1) \
    file_exists(GET_FILE_JSON_PATH(%1))


stock const __JSON_WRAPPER_FIELD_FILE[] = "_file";
stock const __JSON_WRAPPER_FIELD_VALUE[] = "_value";

#define Json_IsWrapper(%1) \
    ( \
        %1 != Invalid_JSON \
        && json_object_has_value(%1, __JSON_WRAPPER_FIELD_FILE, JSONString) \
        && json_object_has_value(%1, __JSON_WRAPPER_FIELD_VALUE) \
        && json_get_parent(%1) == Invalid_JSON \
    )

stock JSON:Json_ParseFromFileEx(const Path[]) {
    new JSON:jFile = json_parse(Path, true, true);
    
    if (jFile == Invalid_JSON) {
        log_amx("[ERROR] Can't read JSON from file '%s'.", Path);
        return Invalid_JSON;
    }

    new JSON:jWrapper = json_init_object();
    json_object_set_string(jWrapper, __JSON_WRAPPER_FIELD_FILE, Path);
    json_object_set_value(jWrapper, __JSON_WRAPPER_FIELD_VALUE, jFile);
    return jFile;
}

stock JSON:Json_DeepFree(&JSON:jValue) {
    new JSON:jParent;
    while (jValue != Invalid_JSON) {
        jParent = json_get_parent(jValue);
        json_free(jValue);
        jValue = jParent;
    }
    return jValue;
}

stock JSON:Json_GetRoot(const JSON:jValue) {
    if (jValue == Invalid_JSON) {
        return Invalid_JSON;
    }

    new JSON:jParent;
    new JSON:jIterator = json_get_parent(jValue);
    while (jIterator != Invalid_JSON) {
        jParent = json_get_parent(jIterator);
        if (jParent == Invalid_JSON) {
            break;
        }
        json_free(jIterator);
        jIterator = jParent;
    }
    return jIterator;
}

stock Json_GetFilePath(const JSON:jValue, Out[], const OutLen) {
    new JSON:jRoot = Json_GetRoot(jValue);
    if (!Json_IsWrapper(jRoot)) {
        return formatex(Out, OutLen, "unknown_file");
    }

    return json_object_get_string(jRoot, __JSON_WRAPPER_FIELD_FILE, Out, OutLen);
}

new stock const __Json_LogForFile_templateFile[] = "[JSON][%s] File: %s";
new stock const __Json_LogForFile_templateMessage[] = "[JSON][%s] Message: %s";
stock Json_LogForFile(const JSON:jValue, const sPrefix[], const sMessage[], any:...) {
    static sFilePath[PLATFORM_MAX_PATH];
    Json_GetFilePath(jValue, sFilePath, charsmax(sFilePath));
    log_amx(__Json_LogForFile_templateFile, sPrefix, sFilePath);

    static sFormattedMessage[1024];
    if (numargs() >= 4) {
        vformat(sFormattedMessage, charsmax(sFormattedMessage), sMessage, 4);
        log_amx(__Json_LogForFile_templateMessage, sPrefix, sFormattedMessage);
    } else {
        log_amx(__Json_LogForFile_templateMessage, sPrefix, sMessage);
    }
}

stock __CMD_NULL_ARG = 0;
stock CMD_INIT_PARAMS() {
    __CMD_NULL_ARG = 0;
    static Cmd[32];
    read_argv(__CMD_NULL_ARG, Cmd, charsmax(Cmd));
    if (equali(Cmd, "say", 3)) {
        __CMD_NULL_ARG++;
    }
}

#define CMD_ARG(%1) \
    (__CMD_NULL_ARG + %1)

#define CMD_ARG_NUM() \
    (read_argc() - __CMD_NULL_ARG - 1)

stock static const JSON_FILE_VALUE_PREFIX[] = "File:";
stock __JSON_LAST_FILE_OBJECT[PLATFORM_MAX_PATH];
stock JSON:JSON_GET_FILE_OR_OBJECT(JSON:Value) {
    __JSON_LAST_FILE_OBJECT[0] = 0;

    if (!json_is_string(Value)) {
        return Value;
    }
    
    new FileName[PLATFORM_MAX_PATH];
    json_get_string(Value, FileName, charsmax(FileName));
    if (!equali(JSON_FILE_VALUE_PREFIX, FileName, charsmax(JSON_FILE_VALUE_PREFIX))) {
        return Value;
    }

    json_free(Value);

    formatex(__JSON_LAST_FILE_OBJECT, charsmax(__JSON_LAST_FILE_OBJECT), FileName[charsmax(JSON_FILE_VALUE_PREFIX)]);

    Value = GET_FILE_JSON(__JSON_LAST_FILE_OBJECT);
    if (Value == Invalid_JSON) {
        log_amx("[WARNING] JSON syntax error in file `%s`.", __JSON_LAST_FILE_OBJECT);        
    }

    return Value;
}

#define GetLastJsonObjectFile() \
    __JSON_LAST_FILE_OBJECT

#define HasLastJsonObjectFile() \
    bool:(GetLastJsonObjectFile()[0])

#define SaveLastJsonObjectFile(%1) \
    new %1[PLATFORM_MAX_PATH];copy(%1, charsmax(%1), GetLastJsonObjectFile())

stock bool:Json_IsRef(const JSON:jValue, FileName[] = NULL_STRING, const Len = 0) {
    if (!json_is_string(jValue)) {
        return false;
    }

    new Str[128];
    json_get_string(jValue, Str, charsmax(Str));
    if (!equali(JSON_FILE_VALUE_PREFIX, Str, charsmax(JSON_FILE_VALUE_PREFIX))) {
        return false;
    }

    if (Len > 0) {
        copy(FileName, Len, Str[charsmax(JSON_FILE_VALUE_PREFIX)]);
    }
    
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

stock Array:json_get_strings_list(const JSON:jValue, const iStringsLen = MAX_FMT_LENGTH, Array:aList = Invalid_Array) {
    static sBuffer[MAX_FMT_LENGTH];
    ArrayCreateIfNotCreated(aList, iStringsLen, json_get_count(jValue));
    
    if (json_is_string(jValue)) {
        json_get_string(jValue, sBuffer, iStringsLen - 1);
        ArrayPushString(aList, sBuffer);
    } else if (json_is_array(jValue)) {
        for (new i = 0, len = json_array_get_count(jValue); i < len; i++) {
            json_array_get_string(jValue, i, sBuffer, iStringsLen - 1);
            ArrayPushString(aList, sBuffer);
        }
    }

    return aList;
}

stock Array:json_object_get_strings_list(
    const JSON:jObj,
    const sKey[],
    const iStringLen = MAX_FMT_LENGTH,
    Array:aList = Invalid_Array,
    const bool:bDotNot = false
) {
    new JSON:jValue = json_object_get_value(jObj, sKey, bDotNot);
    aList = json_get_strings_list(jValue, iStringLen, aList);
    json_free(jValue);
    return aList;
}

#define BitSet(%1,%2) \
    %1 |= (1 << (%2 - 1))

#define BitReset(%1,%2) \
    %1 &= ~(1 << (%2 - 1))

#define BitIs(%1,%2) \
    (bool:(%1 & (1 << (%2 - 1))))

stock bool:HasUserFlagsStr(const UserId, const sFlags[], const bool:bStrict = false) {
    return HasUserFlags(UserId, read_flags(sFlags), bStrict);
}

stock bool:HasUserFlags(const UserId, const bitFlags, const bool:bStrict = false) {
    return HasBits(get_user_flags(UserId), bitFlags, bStrict);
}

stock bool:HasBits(const bits, const bitSearch, const bool:bStrict = false) {
    new bitRes = bits & bitSearch;
    return bStrict
        ? bitRes == bitSearch
        : bitRes != 0
}

// []GetUserName(const UserId)
#define GetUserName(%1) \
    fmt(__USER_NAME_TEMPLATE_STR,%1)

// bool:IsEqualUserName(const UserId, const sName[])
#define IsEqualUserName(%1,%2) \
    equal(GetUserName(%1), %2)

stock GetWeekDayIdByName(const sWeekDayName[]) {
    if (equali("Вс", sWeekDayName) || equali("Воскресенье", sWeekDayName) || equali("Sun", sWeekDayName) || equali("Sunday", sWeekDayName)) {
        return 0;
    } else if (equali("Пн", sWeekDayName) || equali("Понедельник", sWeekDayName) || equali("Mon", sWeekDayName) || equali("Monday", sWeekDayName)) {
        return 1;
    } else if (equali("Вт", sWeekDayName) || equali("Вторник", sWeekDayName) || equali("Tue", sWeekDayName) || equali("Tuesday", sWeekDayName)) {
        return 2;
    } else if (equali("Ср", sWeekDayName) || equali("Среда", sWeekDayName) || equali("Wed", sWeekDayName) || equali("Wednesday", sWeekDayName)) {
        return 3;
    } else if (equali("Чт", sWeekDayName) || equali("Четверг", sWeekDayName) || equali("Thu", sWeekDayName) || equali("Thursday", sWeekDayName)) {
        return 4;
    } else if (equali("Пт", sWeekDayName) || equali("Пятница", sWeekDayName) || equali("Fri", sWeekDayName) || equali("Friday", sWeekDayName)) {
        return 5;
    } else if (equali("Сб", sWeekDayName) || equali("Суббота", sWeekDayName) || equali("Sat", sWeekDayName) || equali("Saturday", sWeekDayName)) {
        return 6;
    } else {
        return -1;
    }
}

stock CreateConstCvar(const sCvarName[], const sCvarValue[]) {
    set_pcvar_string(create_cvar(sCvarName, sCvarValue), sCvarValue);
}

#define CallOnce() CompositeMacros( \
    static bool:__CallOnce_bCalled; \
    if (__CallOnce_bCalled) return; \
    __CallOnce_bCalled = true; \
)

#define CallOnceR(%1) CompositeMacros( \
    static bool:__CallOnce_bCalled; \
    if (__CallOnce_bCalled) return %1; \
    __CallOnce_bCalled = true; \
)

// https://github.com/Nord1cWarr1or/Universal-AFK-Manager/blob/6272afbb8c27f8b7ad770e3036b5960042001e6b/scripting/UAFKManager.sma#L298-L321
stock GetAmxxVersionNum() {
    static iRes;
    if (iRes) {
        return iRes;
    }

    new sAmxxVer[16];
    get_amxx_verstring(sAmxxVer, charsmax(sAmxxVer));

    if (strfind(sAmxxVer, "1.10.0") != -1) {
        iRes = 1100;
    } else if (strfind(sAmxxVer, "1.9.0") != -1) {
        iRes = 190;
    } else if (strfind(sAmxxVer, "1.8.3") != -1) {
        iRes = 183;
    } else if (strfind(sAmxxVer, "1.8.2") != -1) {
        iRes = 182;
    } else {
        iRes = 1;
    }

    return iRes;
}

stock RegisterPlugin(
    const sName[],
    const sVersion[],
    const sAuthor[],
    const sUrl[] = "",
    const sDescription[] = ""
) {
    #if AMXX_VERSION_NUM >= 1100
    if (GetAmxxVersionNum() >= 1100) {
        register_plugin(sName, sVersion, sAuthor, sUrl, sDescription);
        return;
    }
    #endif
    register_plugin(sName, sVersion, sAuthor);
}

#define RegisterPluginByVars() CompositeMacros( \
    if (GetAmxxVersionNum() < 1100) { \
        register_plugin(PluginName, PluginVersion, PluginAuthor); \
    } \
)

// Надо чтобы переменные были определены раньше, что может быть неудобно:)
// stock RegisterPluginByVars() {
//     if (GetAmxxVersionNum() < 1100) {
//         register_plugin(PluginName, PluginVersion, PluginAuthor);
//     }
// }

stock InstantReloadAllWeapons(const UserId) {
    for (new InventorySlotType:iSlot = PRIMARY_WEAPON_SLOT; iSlot <= PISTOL_SLOT; iSlot++) {
        new ItemId = get_member(UserId, m_rgpPlayerItems, iSlot);
        while (!is_nullent(ItemId)) {
            set_member(ItemId, m_Weapon_iClip, rg_get_iteminfo(ItemId, ItemInfo_iMaxClip));
            ItemId = get_member(ItemId, m_pNext);
        }
    }
}

stock InstantReloadActiveWeapon(const UserId) {
    new ItemId = get_member(UserId, m_pActiveItem);
    if (!is_nullent(ItemId)) {
        set_member(ItemId, m_Weapon_iClip, rg_get_iteminfo(ItemId, ItemInfo_iMaxClip));
    }
}

stock bool:StrEqualEx(const sA[], const sB[], const iCount = 0, const bool:bIgnoreCase = false) {
    return bIgnoreCase
        ? equali(sA, sB, iCount)
        : equal(sA, sB, iCount);
}

stock bool:IsUserInBuyZone(const UserId) {
    new Signal[UnifiedSignals];
    get_member(UserId, m_signals, Signal);
    return ((SignalState:Signal[US_State] & SIGNAL_BUY) == SIGNAL_BUY);
}
