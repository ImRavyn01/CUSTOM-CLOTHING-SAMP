#include "natives.h"
#include "plugin.h"
#include "clothing_manager.h"
#include "network.h"
#include "callbacks.h"
#include <cstring>
#include <string>

int Natives::Register(AMX *amx) {
    AMX_NATIVE_INFO natives[] = {
        { "SetPlayerClothes", SetPlayerClothes },
        { "GetPlayerClothes", GetPlayerClothes },
        { "RemovePlayerClothes", RemovePlayerClothes },
        { "RemoveAllPlayerClothes", RemoveAllPlayerClothes },
        { "AddClothingModel", AddClothingModel },
        { "SetPlayerCustomClothes", SetPlayerCustomClothes },
        { "RemoveClothingModel", RemoveClothingModel },
        { "IsClothingModelRegistered", IsClothingModelRegistered },
        { "SyncPlayerClothes", SyncPlayerClothes },
        { "SyncPlayerClothesToPlayer", SyncPlayerClothesToPlayer },
        { "IsPlayerWearingClothes", IsPlayerWearingClothes },
        { "GetPlayerClothingCount", GetPlayerClothingCount },
        { "IsPlayerCJSkin", IsPlayerCJSkin },
        { "GetClothingPluginVersion", GetClothingPluginVersion },
        { "CustomSetPlayerSkin", CustomSetPlayerSkin },
        { NULL, NULL }
    };
    return amx_Register(amx, natives, -1);
}

cell AMX_NATIVE_CALL Natives::SetPlayerClothes(AMX *amx, cell *params) {
    int playerid = params[1];
    int bodypart = params[2];
    char *texture, *model;
    amx_StrParam(amx, params[3], texture);
    amx_StrParam(amx, params[4], model);

    if(!ClothingManager::IsCJSkin(ClothingManager::GetPlayerSkin(playerid))) return 0;

    if (ClothingManager::SetClothing(playerid, bodypart, texture, model)) {
        Network::SendClothingSet(playerid, bodypart, texture, model, -1);
        Callbacks::CallOnPlayerClothesChange(playerid, bodypart, texture, model);
        return 1;
    }
    return 0;
}

cell AMX_NATIVE_CALL Natives::GetPlayerClothes(AMX *amx, cell *params) {
    int playerid = params[1];
    int bodypart = params[2];
    int texLen = params[4];
    int modLen = params[6];
    
    char texture[256];
    char model[256];
    if (ClothingManager::GetClothing(playerid, bodypart, texture, sizeof(texture), model, sizeof(model))) {
        cell *physAddr;
        amx_GetAddr(amx, params[3], &physAddr);
        amx_SetString(physAddr, texture, 0, 0, texLen);
        amx_GetAddr(amx, params[5], &physAddr);
        amx_SetString(physAddr, model, 0, 0, modLen);
        return 1;
    }
    return 0;
}

cell AMX_NATIVE_CALL Natives::RemovePlayerClothes(AMX *amx, cell *params) {
    int playerid = params[1];
    int bodypart = params[2];
    if (ClothingManager::RemoveClothing(playerid, bodypart)) {
        Network::SendClothingRemove(playerid, bodypart);
        Callbacks::CallOnPlayerClothesChange(playerid, bodypart, "", "");
        return 1;
    }
    return 0;
}

cell AMX_NATIVE_CALL Natives::RemoveAllPlayerClothes(AMX *amx, cell *params) {
    int playerid = params[1];
    if (ClothingManager::RemoveAll(playerid)) {
        Network::SendClothingRemoveAll(playerid);
        logprintf("[Clothing] RemoveAllPlayerClothes native returning 1");
        return 1;
    }
    logprintf("[Clothing] RemoveAllPlayerClothes native returning 0");
    return 0;
}

cell AMX_NATIVE_CALL Natives::AddClothingModel(AMX *amx, cell *params) {
    int baseid = params[1];
    int newid = params[2];
    char *dffname, *txdname;
    amx_StrParam(amx, params[3], dffname);
    amx_StrParam(amx, params[4], txdname);

    if (ClothingManager::AddCustomModel(baseid, newid, dffname, txdname)) return 1;
    return 0;
}

cell AMX_NATIVE_CALL Natives::SetPlayerCustomClothes(AMX *amx, cell *params) {
    int playerid = params[1];
    int bodypart = params[2];
    int customModelId = params[3];

    if(!ClothingManager::IsCJSkin(ClothingManager::GetPlayerSkin(playerid))) return 0;

    if (ClothingManager::SetCustomClothing(playerid, bodypart, customModelId)) {
        Network::SendClothingSet(playerid, bodypart, "", "", customModelId);
        Callbacks::CallOnPlayerClothesChange(playerid, bodypart, "CUSTOM", "CUSTOM");
        return 1;
    }
    return 0;
}

cell AMX_NATIVE_CALL Natives::RemoveClothingModel(AMX *amx, cell *params) {
    int customModelId = params[1];
    if (ClothingManager::RemoveCustomModel(customModelId)) return 1;
    return 0;
}

cell AMX_NATIVE_CALL Natives::IsClothingModelRegistered(AMX *amx, cell *params) {
    return ClothingManager::IsCustomModelRegistered(params[1]) ? 1 : 0;
}

cell AMX_NATIVE_CALL Natives::SyncPlayerClothes(AMX *amx, cell *params) {
    int playerid = params[1];
    Network::BroadcastClothingToNearby(playerid);
    return 1;
}

cell AMX_NATIVE_CALL Natives::SyncPlayerClothesToPlayer(AMX *amx, cell *params) {
    int playerid = params[1];
    int forplayerid = params[2];
    Network::SyncPlayerClothesToPlayer(playerid, forplayerid);
    return 1;
}

cell AMX_NATIVE_CALL Natives::IsPlayerWearingClothes(AMX *amx, cell *params) {
    return ClothingManager::IsWearing(params[1], params[2]) ? 1 : 0;
}

cell AMX_NATIVE_CALL Natives::GetPlayerClothingCount(AMX *amx, cell *params) {
    return ClothingManager::GetCount(params[1]);
}

cell AMX_NATIVE_CALL Natives::IsPlayerCJSkin(AMX *amx, cell *params) {
    return ClothingManager::IsCJSkin(ClothingManager::GetPlayerSkin(params[1])) ? 1 : 0;
}

cell AMX_NATIVE_CALL Natives::GetClothingPluginVersion(AMX *amx, cell *params) {
    cell *physAddr;
    amx_GetAddr(amx, params[1], &physAddr);
    amx_SetString(physAddr, CLOTHING_PLUGIN_VERSION, 0, 0, params[2]);
    return 1;
}

cell AMX_NATIVE_CALL Natives::CustomSetPlayerSkin(AMX *amx, cell *params) {
    int playerid = params[1];
    int skinid = params[2];
    ClothingManager::SetPlayerSkin(playerid, skinid);
    // User should call standard SetPlayerSkin afterwards or map it via macro
    return 1;
}
