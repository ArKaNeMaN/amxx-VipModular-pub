#include amxmodx
#include VipModular
#include "VipM/Utils.inc"

#pragma compress 1
#pragma semicolon 1

public stock const PluginName[] = "[VipM][T] JSON Wrapping";
public stock const PluginVersion[] = "1.0.0";
public stock const PluginAuthor[] = "ArKaNeMaN";
public stock const PluginUrl[] = "t.me/arkaneman";

public plugin_init(){
    register_plugin(PluginName, PluginVersion, PluginAuthor);
}

public VipM_IC_OnReadItem(const JSON:jItem, Trie:Params) {
    new Path[PLATFORM_MAX_PATH];
    Json_GetFilePath(jItem, Path, charsmax(Path));

    server_print("[VipM][Test][Json Wrapping] File: '%s'.", Path);

    return VIPM_CONTINUE;
}
