#include <amxmodx>

stock Trie:KeyValueCounter_Reset(&Trie:tCounter) {
    TrieDestroy(tCounter);
    return (tCounter = Invalid_Trie);
}

stock KeyValueCounter_Set(&Trie:tCounter, const sKey[], const iValue) {
    if (tCounter == Invalid_Trie) {
        if (!iValue) {
            return;
        }
        tCounter = TrieCreate();
    }

    TrieSetCell(tCounter, sKey, iValue);
    return (iValue + 1);
}

stock KeyValueCounter_Inc(&Trie:tCounter, const sKey[]) {
    if (tCounter == Invalid_Trie) {
        tCounter = TrieCreate();
        TrieSetCell(tCounter, sKey, 1);
        return 1;
    }

    new iValue = 0;
    if (!TrieGetCell(tCounter, sKey, iValue)) {
        TrieSetCell(tCounter, sKey, 1);
        return 1;
    }

    TrieSetCell(tCounter, sKey, iValue + 1);
    return (iValue + 1);
}

stock KeyValueCounter_Get(&Trie:tCounter, const sKey[]) {
    if (
        tCounter == Invalid_Trie
        || !TrieKeyExists(tCounter, sKey)
    ) {
        return 0;
    }

    new iValue = 0;
    return TrieGetCell(tCounter, sKey, iValue) ? iValue : 0;
}
