#include <amxmodx>
#include <reapi>
#include <json>
#include <ItemsController>

DefaultObjects_ItemType_Money_Register() {
    IC_ItemType_SimpleRegister(
        .name = "Money",
        .onRead = "@OnMoneyRead",
        .onGive = "@OnMoneyGive"
    );
}

@OnMoneyRead(const JSON:instanceJson, const Trie:p) {
    TrieDeleteKey(p, "Name");

    if (!json_object_has_value(instanceJson, "Amount", JSONNumber)) {
        PCJson_LogForFile(instanceJson, "ERROR", "Param `Amount` required for item `Money`.");
        return IC_RET_READ_FAIL;
    }
    TrieSetCell(p, "Amount", json_object_get_number(instanceJson, "Amount"));

    if (json_object_has_value(instanceJson, "GiveType", JSONString)) {
        new sBuffer[32];
        json_object_get_string(instanceJson, "GiveType", sBuffer, charsmax(sBuffer));
        new accountSet = ItemType_Money_StrToAccountSet(sBuffer);
        if (accountSet < 0) {
            PCJson_LogForFile(instanceJson, "ERROR", "Invalid `GiveType` value (%s). Expected `Set` or `Add`.", sBuffer);
        } else {
            TrieSetCell(p, "GiveType", accountSet);
        }
    }

    if (json_object_has_value(instanceJson, "TrackChange", JSONBoolean)) {
        TrieSetCell(p, "TrackChange", json_object_get_bool(instanceJson, "TrackChange"));
    }

    return IC_RET_READ_SUCCESS;
}

@OnMoneyGive(const playerIndex, const Trie:p) {
    rg_add_account(
        playerIndex,
        PCGet_Int(p, "Amount"),
        PCGet_Cell(p, "GiveType", AS_ADD),
        PCGet_Bool(p, "TrackChanges", true)
    );
}

static ItemType_Money_StrToAccountSet(const Str[]) {
    if (equali(Str, "Set")) {
        return _:AS_SET;
    } else if (equali(Str, "Add")) {
        return _:AS_ADD;
    } else {
        return -1;
    }
}
