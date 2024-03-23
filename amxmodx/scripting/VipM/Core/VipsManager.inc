#if defined __vipm__core__vips_manager__included
    #endinput
#endif
#define __vipm__core__vips_manager__included

#include <amxmodx>
#include <regex>
#include "VipM/Forwards"
#include "VipM/ArrayTrieUtils"
#include "VipM/JsonUtils"
#include "VipM/Core/Objects/VipUnit"

static Array:g_aVips = Invalid_Array; // T_VipUnit[]
static g_sRootDir[PLATFORM_MAX_PATH] = "";
static Trie:g_tUserModules[MAX_PLAYERS + 1] = {Invalid_Trie, ...};

VipsManager_Init() {
    CallOnce();

    VipUnit_Init();

    g_aVips = ArrayCreate(1, 1);
    Forwards_Reg("UserUpdated", ET_IGNORE, FP_CELL);
}

VipsManager_VipsCount() {
    return ArraySize(g_aVips);
}

VipsManager_SetRootDir(const sRootDir[]) {
    copy(g_sRootDir, charsmax(g_sRootDir), sRootDir);
}

bool:VipsManager_LoadFromFile(const sFilePath[]) {
    if (!file_exists(sFilePath)) {
        return false;
    }

    new JSON:jVipUnits = Json_ParseFromFileEx(sFilePath, g_sRootDir);
    g_aVips = VipUnit_ReadList(jVipUnits, g_aVips);
    Json_FreeEx(jVipUnits);

    return true;
}

bool:VipsManager_LoadFromFolder(sFolderPath[]) {
    if (!dir_exists(sFolderPath)) {
        return false;
    }

    new sFile[PLATFORM_MAX_PATH], iDirHandler, FileType:iType;
    iDirHandler = open_dir(sFolderPath, sFile, charsmax(sFile), iType);
    if (!iDirHandler) {
        return false;
    }

    new Regex:iRegEx_FileName, ret;
    iRegEx_FileName = regex_compile("(.+).json$", ret, "", 0, "i");

    do {
        if (iType != FileType_File || regex_match_c(sFile, iRegEx_FileName) <= 0) {
            continue;
        }

        VipsManager_LoadFromFile(fmt("%s/%s", sFolderPath, sFile));
    } while (next_file(iDirHandler, sFile, charsmax(sFile), iType));

    regex_free(iRegEx_FileName);
    close_dir(iDirHandler);

    return true;
}

VipsManager_UserReload(const UserId) {
    VipsManager_UserReset(UserId);

    g_tUserModules[UserId] = TrieCreate();
    for (new i = 0, ii = ArraySize(g_aVips); i < ii; ++i) {
        new T_VipUnit:iVipUnit = ArrayGetCell(g_aVips, i);
        if (!VipUnit_CheckUserAccess(iVipUnit, UserId)) {
            continue;
        }

        new Array:aModuleUnits = VipUnit_GetModules(iVipUnit);
        for (new j = 0, jj = ArraySize(aModuleUnits); j < jj; ++j) {
            new T_ModuleUnit:iModuleUnit = ArrayGetCell(aModuleUnits, j);

            new sModuleName[VIPM_MODULES_TYPE_NAME_MAX_LEN]; // TODO: Посмотреть можно ли юзать просто хендлер
            ModuleUnit_GetTypeName(iModuleUnit, sModuleName, charsmax(sModuleName));

            new Trie:tParams = ModuleUnit_GetParams(iModuleUnit);
            if (TrieKeyExists(g_tUserModules[UserId], sModuleName)) {
                new Trie:tOldParams;
                TrieGetCell(g_tUserModules[UserId], sModuleName, tOldParams);

                TrieSetCell(g_tUserModules[UserId], sModuleName, ModuleUnit_Merge(iModuleUnit, tOldParams, tParams));
            } else {
                TrieSetCell(g_tUserModules[UserId], sModuleName, tParams);
            }
        }
    }

    Forwards_CallP("UserUpdated", UserId);
}

VipsManager_UserReset(const UserId) {
    if (g_tUserModules[UserId] == Invalid_Trie) {
        return;
    }

    new TrieIter:Iter = TrieIterCreate(g_tUserModules[UserId]);
    while (!TrieIterEnded(Iter)) {
        new Trie:tParams = Invalid_Trie;
        if (TrieIterGetCell(Iter, tParams)) {
            ModuleUnit_FreeParamsIfTemp(tParams);
        }
        TrieIterNext(Iter);
    }
    TrieIterDestroy(Iter);

    TrieDestroy(g_tUserModules[UserId]);
}

Trie:VipsManager_GetUserParams(const UserId, const T_ModuleType:iModuleType) {
    new Trie:tParams = Invalid_Trie;
    TrieGetCell(g_tUserModules[UserId], ModuleType_iGetName(iModuleType), tParams);
    return tParams;
}
