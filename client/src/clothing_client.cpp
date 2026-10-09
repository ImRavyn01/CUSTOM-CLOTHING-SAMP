#include "clothing_client.h"
#include "samp_structs.h"
#include <fstream>
#include <cstdarg>
#include <map>

std::ofstream g_LogFile;

struct CustomAsset {
    int baseId;
    std::string dff;
    std::string txd;
};

std::map<int, CustomAsset> g_CustomAssets;

void InitClothingClient() {
    char logPath[MAX_PATH] = "clothing_client.log";
    HMODULE hMod = GetModuleHandleA("clothing.asi");
    if (hMod) {
        char fullPath[MAX_PATH];
        if (GetModuleFileNameA(hMod, fullPath, sizeof(fullPath))) {
            char* p = strrchr(fullPath, '\\');
            if (p) {
                *(p + 1) = '\0';
                strcat(fullPath, "clothing_client.log");
                strcpy(logPath, fullPath);
            }
        }
    }
    g_LogFile.open(logPath, std::ios::out);
    LogDebug("Clothing Client Initialized");
}

void LogDebug(const char* fmt, ...) {
    if (!g_LogFile.is_open()) return;
    char buffer[512];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);
    g_LogFile << buffer << std::endl;
    g_LogFile.flush();
}

CPed* GetPlayerPedById(int playerId) {
    CPed* pPed = nullptr;

    if (!g_dwSampBase) {
        pPed = (CPed*)FindPlayerPed(-1);
        return IsValidPed(pPed) ? pPed : nullptr;
    }

    void* pNetGame = *(void**)(g_dwSampBase + SAMP_R2_OFFSET_NETGAME);
    if (!IsValidPedPointer(pNetGame)) {
        pPed = (CPed*)FindPlayerPed(-1);
        return IsValidPed(pPed) ? pPed : nullptr;
    }

    void* pPools = *(void**)((char*)pNetGame + SAMP_R2_NETGAME_POOLS);
    if (!IsValidPedPointer(pPools)) {
        pPed = (CPed*)FindPlayerPed(-1);
        return IsValidPed(pPed) ? pPed : nullptr;
    }

    void* pPlayerPool = *(void**)((char*)pPools + SAMP_R2_POOLS_PLAYERPOOL);
    if (!IsValidPedPointer(pPlayerPool)) {
        pPed = (CPed*)FindPlayerPed(-1);
        return IsValidPed(pPed) ? pPed : nullptr;
    }

    uint16_t localId = *(uint16_t*)((char*)pPlayerPool + SAMP_R2_PLAYERPOOL_LOCALID);
    if (playerId == (int)localId || playerId == -1 || playerId == 65535) {
        pPed = (CPed*)FindPlayerPed(-1);
        return IsValidPed(pPed) ? pPed : nullptr;
    }

    // Remote player ped
    void** pRemotePlayers = (void**)((char*)pPlayerPool + SAMP_R2_PLAYERPOOL_REMOTE);
    if (playerId >= 0 && playerId < 1004 && IsValidPedPointer(pRemotePlayers)) {
        void* pRemote = pRemotePlayers[playerId];
        if (IsValidPedPointer(pRemote)) {
            void* pSampPed = *(void**)pRemote;
            if (IsValidPedPointer(pSampPed)) {
                // If SA-MP CPlayerPed wrapper, m_pPed is at offset 0x2A4
                pPed = *(CPed**)((char*)pSampPed + 0x2A4);
                if (IsValidPed(pPed)) return pPed;

                pPed = (CPed*)pSampPed;
                if (IsValidPed(pPed)) return pPed;
            }
        }
    }

    pPed = (CPed*)FindPlayerPed(-1);
    return IsValidPed(pPed) ? pPed : nullptr;
}

static void ResolveClothesNames(int bodyPart, const char* inTex, const char* inModel, std::string& outTex, std::string& outModel) {
    outTex = inTex ? inTex : "";
    outModel = inModel ? inModel : "";

    // Bare body parts (default CJ mesh)
    if (outTex == "torso" || outTex == "player_torso") {
        outTex = "player_torso";
        outModel = "torso";
    }
    else if (outTex == "legs" || outTex == "player_legs") {
        outTex = "player_legs";
        outModel = "legs";
    }
    else if (outTex == "feet" || outTex == "foot" || outTex == "player_feet") {
        outTex = "player_feet";
        outModel = "feet";
    }
    else if (outTex == "head" || outTex == "player_face") {
        outTex = "player_face";
        outModel = "head";
    }
    // Accessory aliases
    else if (outTex == "glassesblack") {
        outTex = "glasses04";
        if (outModel.empty() || outModel == "glasses04") outModel = "glasses04";
    }
    else if (outTex == "watchtype3") {
        outTex = "watchpink";
        if (outModel.empty() || outModel == "watch") outModel = "watch";
    }
    if (outModel == "necklace") {
        outModel = "neck"; // player.img model is neck.dff
    }
}

void ApplyClothing(int playerId, int bodyPart, const char* textureName, const char* modelName, int customModelId) {
    std::string resolvedTex, resolvedMod;
    ResolveClothesNames(bodyPart, textureName, modelName, resolvedTex, resolvedMod);

    LogDebug("[Clothing] ApplyClothing: player=%d, slot=%d, tex=%s (%s), mod=%s (%s), custom=%d", 
             playerId, bodyPart, textureName, resolvedTex.c_str(), modelName, resolvedMod.c_str(), customModelId);

    CPed* pPed = GetPlayerPedById(playerId);
    if (!pPed) {
        LogDebug("[Clothing] Ped not ready yet for player %d, skipping.", playerId);
        return;
    }

    CPedClothesDesc* pClothesDesc = GetPedClothesDesc(pPed);
    if (!pClothesDesc) {
        LogDebug("[Clothing] ClothesDesc not found for player %d, skipping.", playerId);
        return;
    }

    LogDebug("[Clothing] Setting texture '%s' and model '%s' on slot %d", resolvedTex.c_str(), resolvedMod.c_str(), bodyPart);
    pClothesDesc->SetTextureAndModel(resolvedTex.c_str(), resolvedMod.c_str(), bodyPart);
    CClothes::RebuildPlayer(pPed, true);
    LogDebug("[Clothing] RebuildPlayer completed for player %d!", playerId);
}

void RemoveClothing(int playerId, int bodyPart) {
    LogDebug("[Clothing] RemoveClothing: player=%d, slot=%d", playerId, bodyPart);

    CPed* pPed = GetPlayerPedById(playerId);
    if (!pPed) {
        LogDebug("[Clothing] Ped not ready yet for player %d, skipping.", playerId);
        return;
    }

    CPedClothesDesc* pClothesDesc = GetPedClothesDesc(pPed);
    if (!pClothesDesc) return;

    if (bodyPart == 0) {
        pClothesDesc->SetTextureAndModel("player_torso", "torso", 0);
    } else if (bodyPart == 1) {
        pClothesDesc->SetTextureAndModel("player_face", "head", 1);
    } else if (bodyPart == 2) {
        pClothesDesc->SetTextureAndModel("player_legs", "legs", 2);
    } else if (bodyPart == 3) {
        pClothesDesc->SetTextureAndModel("player_feet", "feet", 3);
    } else {
        pClothesDesc->SetTextureAndModel(0u, 0u, bodyPart);
    }

    CClothes::RebuildPlayer(pPed, true);
    LogDebug("[Clothing] RebuildPlayer completed after removing slot %d", bodyPart);
}

void RemoveAllClothing(int playerId) {
    LogDebug("[Clothing] RemoveAllClothing: player=%d", playerId);

    CPed* pPed = GetPlayerPedById(playerId);
    if (!pPed) {
        LogDebug("[Clothing] Ped not ready yet for player %d (spawn transition), skipping.", playerId);
        return;
    }

    CPedClothesDesc* pClothesDesc = GetPedClothesDesc(pPed);
    if (!pClothesDesc) {
        LogDebug("[Clothing] ClothesDesc not found for player %d, skipping.", playerId);
        return;
    }

    pClothesDesc->Initialise();

    // GTA San Andreas bare skin (underwear/boxers):
    // Slot 0 (Torso): texture "player_torso", model "torso" (bare chest)
    // Slot 1 (Head):  texture "player_face",  model "head"  (default face)
    // Slot 2 (Legs):  texture "player_legs",  model "legs"  (boxer shorts)
    // Slot 3 (Shoes): texture "player_feet",  model "feet"  (bare feet)
    pClothesDesc->SetTextureAndModel("player_torso", "torso", 0);
    pClothesDesc->SetTextureAndModel("player_face", "head", 1);
    pClothesDesc->SetTextureAndModel("player_legs", "legs", 2);
    pClothesDesc->SetTextureAndModel("player_feet", "feet", 3);

    // Clear all other slots (tattoos 4..12, necklace 13, watch 14, glasses 15, hat 16, suit 17)
    for (int slot = 4; slot < 18; slot++) {
        pClothesDesc->SetTextureAndModel(0u, 0u, slot);
    }

    CClothes::RebuildPlayer(pPed, true);
    LogDebug("[Clothing] RebuildPlayer completed after setting bare skin/boxers for player %d", playerId);
}

void SyncAllClothing(int playerId, ClothingSlotData* slots, int slotCount) {
    LogDebug("[Clothing] SyncAllClothing: player=%d, count=%d", playerId, slotCount);

    CPed* pPed = GetPlayerPedById(playerId);
    if (!pPed) return;

    CPedClothesDesc* pClothesDesc = GetPedClothesDesc(pPed);
    if (!pClothesDesc) return;

    pClothesDesc->Initialise();
    for (int i = 0; i < slotCount; i++) {
        pClothesDesc->SetTextureAndModel(slots[i].textureName, slots[i].modelName, slots[i].bodyPart);
    }
    CClothes::RebuildPlayer(pPed, false);
    LogDebug("[Clothing] RebuildPlayer completed after full sync");
}

void RegisterCustomAsset(int assetId, int baseId, const char* dff, const char* txd) {
    LogDebug("[Clothing] Registered custom asset %d -> %s %s", assetId, dff, txd);
    g_CustomAssets[assetId] = { baseId, dff, txd };
}
