#ifndef _CLOTHING_MANAGER_H_
#define _CLOTHING_MANAGER_H_

#include "../../shared/clothing_common.h"
#include <map>
#include <mutex>
#include <string>

struct CustomAssetInfo {
    int baseId;
    std::string dffName;
    std::string txdName;
};

class ClothingManager
{
public:
    static bool SetClothing(int playerid, int bodypart, const char* texture, const char* model);
    static bool GetClothing(int playerid, int bodypart, char* texture, int texLen, char* model, int modLen);
    static bool RemoveClothing(int playerid, int bodypart);
    static bool RemoveAll(int playerid);
    static int GetCount(int playerid);
    static bool IsWearing(int playerid, int bodypart);
    
    static bool AddCustomModel(int baseid, int newid, const char* dffname, const char* txdname);
    static bool RemoveCustomModel(int customModelId);
    static bool IsCustomModelRegistered(int customModelId);
    static bool SetCustomClothing(int playerid, int bodypart, int customModelId);
    
    static ClothingSlotData* GetPlayerSlot(int playerid, int bodypart);
    static const ClothingSlotData* GetPlayerSlots(int playerid);
    static void CleanupPlayer(int playerid);
    static void CleanupAll();

    // Fake skin tracker since we don't have GetPlayerSkin via SAMPGDK
    static void SetPlayerSkin(int playerid, int skinid);
    static int GetPlayerSkin(int playerid);
    static bool IsCJSkin(int skinid) { return skinid == 0; }

private:
    static ClothingSlotData m_PlayerClothes[MAX_SAMP_PLAYERS][MAX_CLOTHING_SLOTS];
    static std::map<int, CustomAssetInfo> m_CustomAssets;
    static int m_PlayerSkins[MAX_SAMP_PLAYERS];
    static std::mutex m_Mutex;
};

#endif // _CLOTHING_MANAGER_H_
