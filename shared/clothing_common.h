/*
 *  Custom CJ Clothing System for SA-MP 0.3.7 R2
 *  Shared Header — clothing_common.h
 *
 *  Defines constants, RPC IDs, and data structures shared
 *  between the server-side plugin and client-side ASI plugin.
 */

#ifndef _CLOTHING_COMMON_H_
#define _CLOTHING_COMMON_H_

#pragma pack(push, 1)

// ============================================================================
//  VERSION
// ============================================================================
#define CLOTHING_PLUGIN_VERSION     "0.0.1"
#define CLOTHING_PROTOCOL_VERSION   1

// ============================================================================
//  CUSTOM RPC IDs (outside SA-MP reserved range 0~170)
// ============================================================================
#define RPC_CLOTHING_SET            200   // Server -> Client: set one clothing slot
#define RPC_CLOTHING_REMOVE         201   // Server -> Client: remove one slot
#define RPC_CLOTHING_REMOVE_ALL     202   // Server -> Client: remove all slots
#define RPC_CLOTHING_SYNC           203   // Server -> Client: bulk sync all slots
#define RPC_CLOTHING_CUSTOM_ASSET   204   // Server -> Client: register custom asset

// ============================================================================
//  BODY PART SLOT CONSTANTS
// ============================================================================
#define CLOTHING_SLOT_TORSO             0
#define CLOTHING_SLOT_HEAD              1
#define CLOTHING_SLOT_LEGS              2
#define CLOTHING_SLOT_SHOES             3
#define CLOTHING_SLOT_TATTOO_LEFT_UA    4   // Left Upper Arm
#define CLOTHING_SLOT_TATTOO_LEFT_LA    5   // Left Lower Arm
#define CLOTHING_SLOT_TATTOO_RIGHT_UA   6   // Right Upper Arm
#define CLOTHING_SLOT_TATTOO_RIGHT_LA   7   // Right Lower Arm
#define CLOTHING_SLOT_TATTOO_BACK       8
#define CLOTHING_SLOT_TATTOO_LEFT_C     9   // Left Chest
#define CLOTHING_SLOT_TATTOO_RIGHT_C    10  // Right Chest
#define CLOTHING_SLOT_TATTOO_STOMACH    11
#define CLOTHING_SLOT_TATTOO_LOWER_B    12  // Lower Back
#define CLOTHING_SLOT_NECKLACE          13
#define CLOTHING_SLOT_WATCH             14
#define CLOTHING_SLOT_GLASSES           15
#define CLOTHING_SLOT_HAT               16
#define CLOTHING_SLOT_EXTRA             17

#define MAX_CLOTHING_SLOTS              18
#define MAX_CLOTHING_TEXTURE_NAME       64
#define MAX_CLOTHING_MODEL_NAME         64

// SA-MP limits
#define MAX_SAMP_PLAYERS                1000
#define INVALID_PLAYER_ID               0xFFFF
#define CJ_SKIN_ID                      0

// ============================================================================
//  NETWORK PACKET STRUCTURES
// ============================================================================

// Single clothing slot data (used in all packets)
struct ClothingSlotData
{
    unsigned char  bodyPart;                              // 0-17
    char           textureName[MAX_CLOTHING_TEXTURE_NAME]; // texture from .txd
    char           modelName[MAX_CLOTHING_MODEL_NAME];     // model from .dff
    int            customModelId;                          // -1 = built-in GTA, >= 0 = custom asset
};

// RPC_CLOTHING_SET payload
struct ClothingPacketSet
{
    unsigned short playerId;        // target player
    ClothingSlotData slot;          // clothing data to set
};

// RPC_CLOTHING_REMOVE payload
struct ClothingPacketRemove
{
    unsigned short playerId;        // target player
    unsigned char  bodyPart;        // slot to remove (0-17)
};

// RPC_CLOTHING_REMOVE_ALL payload
struct ClothingPacketRemoveAll
{
    unsigned short playerId;        // target player
};

// RPC_CLOTHING_SYNC payload (bulk sync)
struct ClothingPacketSync
{
    unsigned short playerId;        // target player
    unsigned char  slotCount;       // number of active slots (0-18)
    ClothingSlotData slots[MAX_CLOTHING_SLOTS]; // all clothing data
};

// RPC_CLOTHING_CUSTOM_ASSET payload
struct ClothingPacketCustomAsset
{
    int            assetId;         // unique custom model ID
    int            baseId;          // base GTA model ID for fallback
    char           dffName[64];     // DFF filename
    char           txdName[64];     // TXD filename
    unsigned int   dffCRC;          // CRC32 hash for verification
    unsigned int   txdCRC;          // CRC32 hash for verification
    unsigned int   dffSize;         // file size in bytes
    unsigned int   txdSize;         // file size in bytes
};

// ============================================================================
//  HELPER: Check if body part ID is valid
// ============================================================================
inline bool IsValidBodyPart(int bodyPart)
{
    return (bodyPart >= 0 && bodyPart < MAX_CLOTHING_SLOTS);
}

// ============================================================================
//  HELPER: Check if skin is CJ (clothing only works on CJ)
// ============================================================================
inline bool IsCJSkin(int skinId)
{
    return (skinId == CJ_SKIN_ID);
}

#pragma pack(pop)

#endif // _CLOTHING_COMMON_H_
