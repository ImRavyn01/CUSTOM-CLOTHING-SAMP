#ifndef _NATIVES_H_
#define _NATIVES_H_

#include "../../sdk/amx/amx.h"

class Natives {
public:
    static int Register(AMX *amx);

private:
    static cell AMX_NATIVE_CALL SetPlayerClothes(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL GetPlayerClothes(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL RemovePlayerClothes(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL RemoveAllPlayerClothes(AMX *amx, cell *params);
    
    static cell AMX_NATIVE_CALL AddClothingModel(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL SetPlayerCustomClothes(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL RemoveClothingModel(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL IsClothingModelRegistered(AMX *amx, cell *params);
    
    static cell AMX_NATIVE_CALL SyncPlayerClothes(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL SyncPlayerClothesToPlayer(AMX *amx, cell *params);
    
    static cell AMX_NATIVE_CALL IsPlayerWearingClothes(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL GetPlayerClothingCount(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL IsPlayerCJSkin(AMX *amx, cell *params);
    static cell AMX_NATIVE_CALL GetClothingPluginVersion(AMX *amx, cell *params);

    static cell AMX_NATIVE_CALL CustomSetPlayerSkin(AMX *amx, cell *params);
};

#endif // _NATIVES_H_
