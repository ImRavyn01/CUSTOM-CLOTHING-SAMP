#include "clothing_manager.h"
#include <cstring>
#include <algorithm>

ClothingSlotData ClothingManager::m_PlayerClothes[MAX_SAMP_PLAYERS][MAX_CLOTHING_SLOTS];
std::map<int, CustomAssetInfo> ClothingManager::m_CustomAssets;
int ClothingManager::m_PlayerSkins[MAX_SAMP_PLAYERS] = {0}; // default to CJ (0)
std::mutex ClothingManager::m_Mutex;

bool ClothingManager::SetClothing(int playerid, int bodypart, const char* texture, const char* model)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return false;
    if(!IsValidBodyPart(bodypart)) return false;

    std::lock_guard<std::mutex> lock(m_Mutex);
    auto& slot = m_PlayerClothes[playerid][bodypart];
    slot.bodyPart = bodypart;
    strncpy(slot.textureName, texture, MAX_CLOTHING_TEXTURE_NAME - 1);
    slot.textureName[MAX_CLOTHING_TEXTURE_NAME - 1] = '\0';
    strncpy(slot.modelName, model, MAX_CLOTHING_MODEL_NAME - 1);
    slot.modelName[MAX_CLOTHING_MODEL_NAME - 1] = '\0';
    slot.customModelId = -1;
    return true;
}

bool ClothingManager::GetClothing(int playerid, int bodypart, char* texture, int texLen, char* model, int modLen)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return false;
    if(!IsValidBodyPart(bodypart)) return false;

    std::lock_guard<std::mutex> lock(m_Mutex);
    auto& slot = m_PlayerClothes[playerid][bodypart];
    if (slot.textureName[0] == '\0' && slot.modelName[0] == '\0' && slot.customModelId == -1) return false;

    strncpy(texture, slot.textureName, texLen);
    texture[texLen - 1] = '\0';
    strncpy(model, slot.modelName, modLen);
    model[modLen - 1] = '\0';
    return true;
}

bool ClothingManager::RemoveClothing(int playerid, int bodypart)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return false;
    if(!IsValidBodyPart(bodypart)) return false;

    std::lock_guard<std::mutex> lock(m_Mutex);
    memset(&m_PlayerClothes[playerid][bodypart], 0, sizeof(ClothingSlotData));
    m_PlayerClothes[playerid][bodypart].customModelId = -1;
    return true;
}

bool ClothingManager::RemoveAll(int playerid)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return false;

    std::lock_guard<std::mutex> lock(m_Mutex);
    for(int i = 0; i < MAX_CLOTHING_SLOTS; i++) {
        memset(&m_PlayerClothes[playerid][i], 0, sizeof(ClothingSlotData));
        m_PlayerClothes[playerid][i].customModelId = -1;
    }
    return true;
}

int ClothingManager::GetCount(int playerid)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return 0;
    int count = 0;
    std::lock_guard<std::mutex> lock(m_Mutex);
    for(int i = 0; i < MAX_CLOTHING_SLOTS; i++) {
        if(m_PlayerClothes[playerid][i].textureName[0] != '\0' || m_PlayerClothes[playerid][i].modelName[0] != '\0' || m_PlayerClothes[playerid][i].customModelId != -1) {
            count++;
        }
    }
    return count;
}

bool ClothingManager::IsWearing(int playerid, int bodypart)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return false;
    if(!IsValidBodyPart(bodypart)) return false;
    std::lock_guard<std::mutex> lock(m_Mutex);
    return (m_PlayerClothes[playerid][bodypart].textureName[0] != '\0' || m_PlayerClothes[playerid][bodypart].modelName[0] != '\0' || m_PlayerClothes[playerid][bodypart].customModelId != -1);
}

bool ClothingManager::AddCustomModel(int baseid, int newid, const char* dffname, const char* txdname)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    m_CustomAssets[newid] = {baseid, dffname, txdname};
    return true;
}

bool ClothingManager::RemoveCustomModel(int customModelId)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_CustomAssets.erase(customModelId) > 0;
}

bool ClothingManager::IsCustomModelRegistered(int customModelId)
{
    std::lock_guard<std::mutex> lock(m_Mutex);
    return m_CustomAssets.find(customModelId) != m_CustomAssets.end();
}

bool ClothingManager::SetCustomClothing(int playerid, int bodypart, int customModelId)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return false;
    if(!IsValidBodyPart(bodypart)) return false;

    std::lock_guard<std::mutex> lock(m_Mutex);
    if(m_CustomAssets.find(customModelId) == m_CustomAssets.end()) return false;

    auto& slot = m_PlayerClothes[playerid][bodypart];
    slot.bodyPart = bodypart;
    slot.textureName[0] = '\0';
    slot.modelName[0] = '\0';
    slot.customModelId = customModelId;
    return true;
}

ClothingSlotData* ClothingManager::GetPlayerSlot(int playerid, int bodypart)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return nullptr;
    if(!IsValidBodyPart(bodypart)) return nullptr;
    return &m_PlayerClothes[playerid][bodypart];
}

const ClothingSlotData* ClothingManager::GetPlayerSlots(int playerid)
{
    if(playerid < 0 || playerid >= MAX_SAMP_PLAYERS) return nullptr;
    return m_PlayerClothes[playerid];
}

void ClothingManager::CleanupPlayer(int playerid)
{
    RemoveAll(playerid);
    m_PlayerSkins[playerid] = 0;
}

void ClothingManager::CleanupAll()
{
    for(int i = 0; i < MAX_SAMP_PLAYERS; i++) CleanupPlayer(i);
    m_CustomAssets.clear();
}

void ClothingManager::SetPlayerSkin(int playerid, int skinid)
{
    if(playerid >= 0 && playerid < MAX_SAMP_PLAYERS)
        m_PlayerSkins[playerid] = skinid;
}

int ClothingManager::GetPlayerSkin(int playerid)
{
    if(playerid >= 0 && playerid < MAX_SAMP_PLAYERS)
        return m_PlayerSkins[playerid];
    return -1;
}
