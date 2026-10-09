#ifndef _SAMP_STRUCTS_H_
#define _SAMP_STRUCTS_H_

#include <cstdint>
#include <cstring>
#include <windows.h>
#include "game_sa.h"

#pragma pack(push, 1)

struct RPCParameters
{
    unsigned char* input;              // Pointer to raw packet data
    unsigned int   numberOfBitsOfData; // Size of data in bits
    void*          sender;             // Sender identification
    void*          recipient;          // Recipient identification
};

typedef void (__cdecl* RPCFunction)(RPCParameters* rpcParams);

// Simple BitStream reader for extracting data from incoming RPCs
class BitStream
{
public:
    unsigned char* data;
    unsigned int   readOffsetBits;
    unsigned int   dataLengthBytes;

    BitStream(unsigned char* _data, unsigned int _numBits)
        : data(_data)
        , readOffsetBits(0)
        , dataLengthBytes((_numBits + 7) / 8)
    {
    }

    bool ReadBytes(void* dest, int numBytes)
    {
        unsigned int byteOffset = readOffsetBits / 8;
        if (byteOffset + numBytes > dataLengthBytes)
            return false;
        memcpy(dest, data + byteOffset, numBytes);
        readOffsetBits += numBytes * 8;
        return true;
    }

    template<typename T>
    bool Read(T& value)
    {
        return ReadBytes(&value, sizeof(T));
    }
};

#pragma pack(pop)

// SA-MP 0.3.7-R2 static offsets relative to samp.dll base
#define SAMP_R2_OFFSET_NETGAME          0x21A100
#define SAMP_R2_OFFSET_HANDLE_RPC       0x373D0
#define SAMP_R2_OFFSET_REGISTER_RPC     0x4DCA0

// Inside CNetGame / stSAMP:
#define SAMP_R2_NETGAME_RAKCLIENT       0x3C1
#define SAMP_R2_NETGAME_POOLS           0x3C5
#define SAMP_R2_NETGAME_GAMESTATE       0x3B9

// Inside pPools:
#define SAMP_R2_POOLS_PLAYERPOOL        0x08

// Inside pPlayerPool:
#define SAMP_R2_PLAYERPOOL_LOCALID      0x04
#define SAMP_R2_PLAYERPOOL_REMOTE       0x18

// Inside RakClient:
#define SAMP_R2_RAKCLIENT_RPCTABLE      0x801

extern uintptr_t g_dwSampBase;

#endif // _SAMP_STRUCTS_H_
