/*
 *  Custom CJ Clothing System — Network Module
 *  network.cpp
 *
 *  COMPLETELY REWRITTEN based on YSF (Y_Less Server Framework) source code.
 *  YSF is the most widely-used SA-MP plugin that hooks RakServer.
 *
 *  Key references:
 *  - YSF src/CFunctions.h: RPC function pointer typedef
 *  - YSF src/CFunctions.cpp: How to extract and call RPC from vtable
 *  - YSF src/CAddresses.h: Vtable index 32 for RPC on Windows
 *  - YSF lib/raknet/NetworkTypes.h: PlayerID, PacketReliability enums
 *  - YSF lib/raknet/BitStream.h: BitStream layout (256 byte stack, no pack(1))
 *
 *  Changes from previous implementation:
 *  1. PacketReliability enum now starts at 6 (SA-MP's internal values)
 *     - We were passing 3 (INVALID), should be 9 (RELIABLE_ORDERED)
 *  2. BitStream no longer uses #pragma pack(push, 1)
 *  3. BitStream stack buffer is 256 bytes (was 512)
 *  4. BitStream copyData defaults to true (was false)
 *  5. uniqueID is BYTE* (was int*)
 *  6. Uses direct function pointer from vtable (was fake virtual interface class)
 *  7. orderingChannel is unsigned (was char)
 */

#include "network.h"
#include "plugin.h"
#include "clothing_manager.h"
#include <cstring>

#if defined(_WIN32)
#include <windows.h>
#include <psapi.h>
#define THISCALL __thiscall
#else
#include <unistd.h>
#include <dlfcn.h>
#define THISCALL
#endif

// ============================================================================
//  BitStream Write Implementation
// ============================================================================

void RakNet::BitStream::Write(const char* input, int numberOfBytes)
{
    int byteOffset = numberOfBitsUsed >> 3;
    if (byteOffset + numberOfBytes <= BITSTREAM_STACK_ALLOCATION_SIZE)
    {
        memcpy(data + byteOffset, input, numberOfBytes);
        numberOfBitsUsed += numberOfBytes * 8;
    }
}

// ============================================================================
//  YSF-Style RakServer Function Pointer Typedefs
//  (Exact copy from YSF src/CFunctions.h)
// ============================================================================

// RPC: vtable index 32 on Windows, 35 on Linux
typedef bool (THISCALL *RakNet__RPC_t)(
    void* ppRakServer,
    unsigned char* uniqueID,            // BYTE* — NOT int*!
    RakNet::BitStream* parameters,
    PacketPriority priority,
    PacketReliability reliability,
    unsigned orderingChannel,           // unsigned — NOT char!
    PlayerID playerId,
    bool broadcast,
    bool shiftTimestamp
);

// GetPlayerIDFromIndex: vtable index 58 on Windows
typedef PlayerID (THISCALL *RakNet__GetPlayerIDFromIndex_t)(
    void* ppRakServer,
    int index
);

// Vtable offsets from YSF src/CAddresses.h
#ifdef _WIN32
    #define RAKNET_RPC_OFFSET                       32
    #define RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET    58
#else
    #define RAKNET_RPC_OFFSET                       35
    #define RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET    59
#endif

// ============================================================================
//  Cached function pointers (extracted from vtable once, reused forever)
// ============================================================================
static RakNet__RPC_t                    pfn_RakNet_RPC = nullptr;
static RakNet__GetPlayerIDFromIndex_t   pfn_RakNet_GetPlayerIDFromIndex = nullptr;

// ============================================================================
//  Lazy initialization state
// ============================================================================
static bool g_NetworkInitialized = false;
static bool g_NetworkInitAttempted = false;

// ============================================================================
//  RakServer Discovery & Function Pointer Extraction
//  (Modeled after YSF src/CFunctions.cpp PostInitialize)
// ============================================================================

static void EnsureNetworkReady()
{
    if (g_NetworkInitAttempted) return;
    g_NetworkInitAttempted = true;

    logprintf("[Clothing] Network: Initializing RakServer connection...");

    // Step 1: Get pRakServer from ppData[0xE2] — same as YSF
    if (pfn_GetRakServer)
    {
        void* candidate = pfn_GetRakServer();
        if (candidate)
        {
            pRakServer = candidate;
            logprintf("[Clothing] Network: RakServer obtained at 0x%08X", (uintptr_t)candidate);
        }
    }

    if (!pRakServer)
    {
        logprintf("[Clothing] Network: ERROR — Could not obtain RakServer pointer!");
        return;
    }

    // Step 2: Extract vtable — exactly like YSF:
    //   int *pRakServer_VTBL = ((int*)(*(void**)pRakServer));
    int* pRakServer_VTBL = (int*)(*(void**)pRakServer);

    if (!pRakServer_VTBL)
    {
        logprintf("[Clothing] Network: ERROR — RakServer vtable is NULL!");
        return;
    }

    // Step 3: Extract function pointers from vtable — exactly like YSF:
    //   POINT_TO_MEMBER(RakNet__RPC, pRakServer_VTBL[RAKNET_RPC_OFFSET]);
    pfn_RakNet_RPC = (RakNet__RPC_t)pRakServer_VTBL[RAKNET_RPC_OFFSET];
    pfn_RakNet_GetPlayerIDFromIndex = (RakNet__GetPlayerIDFromIndex_t)pRakServer_VTBL[RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET];

    if (!pfn_RakNet_RPC)
    {
        logprintf("[Clothing] Network: ERROR — RPC function pointer is NULL!");
        return;
    }

    g_NetworkInitialized = true;

    logprintf("[Clothing] Network: Successfully initialized!");
    logprintf("[Clothing] Network: RPC func = 0x%08X (vtable[%d])",
              (uintptr_t)pfn_RakNet_RPC, RAKNET_RPC_OFFSET);
    logprintf("[Clothing] Network: GetPlayerIDFromIndex func = 0x%08X (vtable[%d])",
              (uintptr_t)pfn_RakNet_GetPlayerIDFromIndex, RAKNET_GET_PLAYERID_FROM_INDEX_OFFSET);
}

// ============================================================================
//  Initialization (called from Load)
// ============================================================================

bool Network::Initialize()
{
    logprintf("[Clothing] Network: Initialized (RakServer discovery deferred).");
    g_NetworkInitialized = false;
    g_NetworkInitAttempted = false;
    return true;
}

// ============================================================================
//  RPC Sending — Modeled exactly after YSF CSAMPFunctions::RPC()
//
//  YSF code (src/CFunctions.cpp):
//    bool CSAMPFunctions::RPC(BYTE* uniqueID, RakNet::BitStream* parameters,
//        PacketPriority priority, PacketReliability reliability,
//        unsigned orderingChannel, PlayerID playerId,
//        bool broadcast, bool shiftTimestamp)
//    {
//        return pfn__RakNet__RPC(pRakServer, uniqueID, parameters,
//            priority, reliability, orderingChannel,
//            playerId, broadcast, shiftTimestamp);
//    }
// ============================================================================

void Network::SendRPC(unsigned char rpcId, RakNet::BitStream* bitStream,
                      PacketPriority priority, PacketReliability reliability,
                      unsigned orderingChannel, PlayerID playerId,
                      bool broadcast, bool shiftTimestamp)
{
    EnsureNetworkReady();

    if (!g_NetworkInitialized || !pfn_RakNet_RPC || !pRakServer)
    {
        logprintf("[Clothing] SendRPC: Network not ready, skipping RPC %d", rpcId);
        return;
    }

    // YSF passes uniqueID as BYTE* (pointer to a BYTE containing the RPC ID)
    // Use a static variable so the pointer remains valid
    static unsigned char s_rpcId;
    s_rpcId = rpcId;

    logprintf("[Clothing] SendRPC: id=%d priority=%d reliability=%d broadcast=%d",
              rpcId, priority, reliability, broadcast);

    bool result = pfn_RakNet_RPC(
        pRakServer,
        &s_rpcId,
        bitStream,
        priority,
        reliability,
        orderingChannel,
        playerId,
        broadcast,
        shiftTimestamp
    );

    logprintf("[Clothing] SendRPC: id=%d result=%s", rpcId, result ? "OK" : "FAIL");
}

// ============================================================================
//  High-level Send Functions
// ============================================================================

void Network::SendClothingSet(int playerid, int bodypart, const char* texture,
                               const char* model, int customModelId)
{
    RakNet::BitStream bs;
    ClothingPacketSet packet;
    memset(&packet, 0, sizeof(packet));

    packet.playerId = (unsigned short)playerid;
    packet.slot.bodyPart = (unsigned char)bodypart;
    if (texture) strncpy(packet.slot.textureName, texture, MAX_CLOTHING_TEXTURE_NAME - 1);
    if (model)   strncpy(packet.slot.modelName, model, MAX_CLOTHING_MODEL_NAME - 1);
    packet.slot.customModelId = customModelId;

    bs.Write((const char*)&packet, sizeof(packet));
    SendRPC(RPC_CLOTHING_SET, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
            UNASSIGNED_PLAYER_ID, true, false);
}

void Network::SendClothingRemove(int playerid, int bodypart)
{
    RakNet::BitStream bs;
    ClothingPacketRemove packet;
    memset(&packet, 0, sizeof(packet));

    packet.playerId = (unsigned short)playerid;
    packet.bodyPart = (unsigned char)bodypart;

    bs.Write((const char*)&packet, sizeof(packet));
    SendRPC(RPC_CLOTHING_REMOVE, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
            UNASSIGNED_PLAYER_ID, true, false);
}

void Network::SendClothingRemoveAll(int playerid)
{
    RakNet::BitStream bs;
    ClothingPacketRemoveAll packet;
    memset(&packet, 0, sizeof(packet));

    packet.playerId = (unsigned short)playerid;

    bs.Write((const char*)&packet, sizeof(packet));
    SendRPC(RPC_CLOTHING_REMOVE_ALL, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
            UNASSIGNED_PLAYER_ID, true, false);
}

void Network::SendClothingSync(int playerid)
{
    const ClothingSlotData* slots = ClothingManager::GetPlayerSlots(playerid);
    if (!slots) return;

    RakNet::BitStream bs;
    ClothingPacketSync packet;
    memset(&packet, 0, sizeof(packet));

    packet.playerId = (unsigned short)playerid;
    packet.slotCount = 0;

    for (int i = 0; i < MAX_CLOTHING_SLOTS; i++)
    {
        if (slots[i].textureName[0] != '\0' ||
            slots[i].modelName[0] != '\0' ||
            slots[i].customModelId != -1)
        {
            packet.slots[packet.slotCount] = slots[i];
            packet.slotCount++;
        }
    }

    int dataSize = sizeof(packet.playerId) + sizeof(packet.slotCount)
                   + (packet.slotCount * sizeof(ClothingSlotData));
    bs.Write((const char*)&packet, dataSize);
    SendRPC(RPC_CLOTHING_SYNC, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
            UNASSIGNED_PLAYER_ID, true, false);
}

void Network::SyncPlayerClothesToPlayer(int playerid, int forplayerid)
{
    const ClothingSlotData* slots = ClothingManager::GetPlayerSlots(playerid);
    if (!slots) return;

    RakNet::BitStream bs;
    ClothingPacketSync packet;
    memset(&packet, 0, sizeof(packet));

    packet.playerId = (unsigned short)playerid;
    packet.slotCount = 0;

    for (int i = 0; i < MAX_CLOTHING_SLOTS; i++)
    {
        if (slots[i].textureName[0] != '\0' ||
            slots[i].modelName[0] != '\0' ||
            slots[i].customModelId != -1)
        {
            packet.slots[packet.slotCount] = slots[i];
            packet.slotCount++;
        }
    }

    // Get the specific player's PlayerID from their index
    PlayerID targetPlayer = UNASSIGNED_PLAYER_ID;
    if (pfn_RakNet_GetPlayerIDFromIndex)
    {
        targetPlayer = pfn_RakNet_GetPlayerIDFromIndex(pRakServer, forplayerid);
    }

    int dataSize = sizeof(packet.playerId) + sizeof(packet.slotCount)
                   + (packet.slotCount * sizeof(ClothingSlotData));
    bs.Write((const char*)&packet, dataSize);
    SendRPC(RPC_CLOTHING_SYNC, &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0,
            targetPlayer, false, false);
}

void Network::BroadcastClothingToNearby(int playerid)
{
    SendClothingSync(playerid);
}
