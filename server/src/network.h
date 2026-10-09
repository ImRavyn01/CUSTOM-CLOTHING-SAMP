/*
 *  Custom CJ Clothing System — Network Header
 *  network.h
 *
 *  Rewritten to match YSF's proven RakServer interface exactly.
 *  All enums, structs, and function signatures are verified against
 *  the YSF source code (the most widely-used SA-MP server plugin).
 */

#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <cstdint>
#include <cstring>
#include <cstdlib>
#include "../../shared/clothing_common.h"

// ============================================================================
//  RakNet Enums — MUST match SA-MP 0.3.7-R2 internal values
//  (Verified from YSF lib/raknet/NetworkTypes.h)
// ============================================================================

enum PacketPriority
{
    SYSTEM_PRIORITY,    // 0
    HIGH_PRIORITY,      // 1
    MEDIUM_PRIORITY,    // 2
    LOW_PRIORITY,       // 3
    NUMBER_OF_PRIORITIES
};

enum PacketReliability
{
    UNRELIABLE = 6,         // SA-MP starts reliability enum at 6!
    UNRELIABLE_SEQUENCED,   // 7
    RELIABLE,               // 8
    RELIABLE_ORDERED,       // 9
    RELIABLE_SEQUENCED      // 10
};

// ============================================================================
//  PlayerID — Exact layout from YSF lib/raknet/NetworkTypes.h
// ============================================================================

#pragma pack(push, 1)
struct PlayerID
{
    unsigned int binaryAddress;
    unsigned short port;
};
#pragma pack(pop)

const PlayerID UNASSIGNED_PLAYER_ID = { 0xFFFFFFFF, 0xFFFF };

// ============================================================================
//  BitStream — Layout matches YSF lib/raknet/BitStream.h EXACTLY
//  NO #pragma pack on the class — uses default MSVC alignment
//  Stack buffer is 256 bytes (not 512)
//  copyData defaults to true (not false)
// ============================================================================

#define BITSTREAM_STACK_ALLOCATION_SIZE 256

namespace RakNet
{
    class BitStream
    {
    public:
        BitStream()
        {
            numberOfBitsUsed      = 0;
            numberOfBitsAllocated = BITSTREAM_STACK_ALLOCATION_SIZE * 8;
            readOffset            = 0;
            data                  = (unsigned char*)stackData;
            copyData              = true;  // YSF sets this to true
            memset(stackData, 0, sizeof(stackData));
        }

        ~BitStream()
        {
            // Only free if we allocated on heap (we never do in our usage)
            if (copyData && numberOfBitsAllocated > (BITSTREAM_STACK_ALLOCATION_SIZE << 3))
            {
                free(data);
            }
        }

        void Write(const char* input, int numberOfBytes);
        
        template <class T>
        void Write(T var)
        {
            int byteOffset = numberOfBitsUsed >> 3;
            if (byteOffset + (int)sizeof(T) <= BITSTREAM_STACK_ALLOCATION_SIZE)
            {
                memcpy(data + byteOffset, &var, sizeof(T));
                numberOfBitsUsed += sizeof(T) * 8;
            }
        }

        void Reset() { numberOfBitsUsed = 0; readOffset = 0; }

        int GetNumberOfBitsUsed() const { return numberOfBitsUsed; }
        int GetNumberOfBytesUsed() const { return (numberOfBitsUsed + 7) >> 3; }
        unsigned char* GetData() const { return data; }

    // PRIVATE fields — but must be at the TOP of the class (no vtable!)
    // Layout order verified from YSF BitStream.h:
    //   int numberOfBitsUsed;        // +0x00
    //   int numberOfBitsAllocated;   // +0x04
    //   int readOffset;              // +0x08
    //   unsigned char *data;         // +0x0C
    //   bool copyData;               // +0x10
    //   unsigned char stackData[256] // +0x14 (with default alignment padding)
    //private:  // Keep public so we can verify offsets during debugging
        int numberOfBitsUsed;
        int numberOfBitsAllocated;
        int readOffset;
        unsigned char *data;
        bool copyData;
        unsigned char stackData[BITSTREAM_STACK_ALLOCATION_SIZE];
    };
}

// ============================================================================
//  Network class — Public API
// ============================================================================

class Network
{
public:
    static bool Initialize();
    static void SendClothingSet(int playerid, int bodypart, const char* texture, const char* model, int customModelId);
    static void SendClothingRemove(int playerid, int bodypart);
    static void SendClothingRemoveAll(int playerid);
    static void SendClothingSync(int playerid);
    static void SyncPlayerClothesToPlayer(int playerid, int forplayerid);
    static void BroadcastClothingToNearby(int playerid);

private:
    static void SendRPC(unsigned char rpcId, RakNet::BitStream* bitStream,
                        PacketPriority priority, PacketReliability reliability,
                        unsigned orderingChannel, PlayerID playerId,
                        bool broadcast, bool shiftTimestamp);
};

#endif // _NETWORK_H_
