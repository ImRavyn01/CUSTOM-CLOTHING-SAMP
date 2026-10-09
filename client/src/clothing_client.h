#ifndef _CLOTHING_CLIENT_H_
#define _CLOTHING_CLIENT_H_

#include <string>
#include "../../shared/clothing_common.h"
#include "game_sa.h"

// Initialize logging and custom asset systems
void InitClothingClient();

// Get local or remote ped
CPed* GetLocalPlayerPed();
CPed* GetRemotePlayerPed(int playerId);

// Apply clothing functions
void ApplyClothing(int playerId, int bodyPart, const char* textureName, const char* modelName, int customModelId);
void RemoveClothing(int playerId, int bodyPart);
void RemoveAllClothing(int playerId);
void SyncAllClothing(int playerId, ClothingSlotData* slots, int slotCount);
void RegisterCustomAsset(int assetId, int baseId, const char* dff, const char* txd);

void LogDebug(const char* fmt, ...);

#endif // _CLOTHING_CLIENT_H_
