#include "callbacks.h"
#include "plugin.h"
#include "../../sdk/amx/amx.h"

void Callbacks::CallOnPlayerClothesChange(int playerid, int bodypart, const char* texture, const char* model) {
    for (AMX* amx : pAMXFunctions) {
        int idx;
        if (!amx_FindPublic(amx, "OnPlayerClothesChange", &idx)) {
            cell amx_addr1, *phys_addr1;
            cell amx_addr2, *phys_addr2;
            amx_PushString(amx, &amx_addr2, &phys_addr2, model, 0, 0);
            amx_PushString(amx, &amx_addr1, &phys_addr1, texture, 0, 0);
            amx_Push(amx, bodypart);
            amx_Push(amx, playerid);
            cell ret;
            amx_Exec(amx, &ret, idx);
            amx_Release(amx, amx_addr1);
            amx_Release(amx, amx_addr2);
        }
    }
}

void Callbacks::CallOnPlayerClothesLoaded(int playerid, int slotCount) {
    for (AMX* amx : pAMXFunctions) {
        int idx;
        if (!amx_FindPublic(amx, "OnPlayerClothesLoaded", &idx)) {
            amx_Push(amx, slotCount);
            amx_Push(amx, playerid);
            cell ret;
            amx_Exec(amx, &ret, idx);
        }
    }
}
