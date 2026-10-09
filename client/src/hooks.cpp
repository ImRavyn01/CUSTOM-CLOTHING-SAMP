#include "hooks.h"
#include "samp_structs.h"
#include "clothing_client.h"
#include "rpc_handler.h"
#include <windows.h>
#include <psapi.h>

uintptr_t g_dwSampBase = 0;
static HMODULE g_hSampDll = nullptr;

static unsigned char g_origHandleRpcBytes[5];
static bool g_bHookInstalled = false;
static bool g_bEngineHooksInstalled = false;
static void* g_lastRegisteredRakClient = nullptr;

typedef bool (__thiscall *HandleRPCPacket_t)(void* pRakClient, unsigned char* data, int length, int playerId_addr, short playerId_port);

static bool __fastcall HookedHandleRPCPacket(void* pRakClient, void* edx, unsigned char* data, int length, int playerId_addr, short playerId_port)
{
    // Ensure RPCs are registered on this active RakClient instance
    if (pRakClient && pRakClient != g_lastRegisteredRakClient)
    {
        g_lastRegisteredRakClient = pRakClient;
        LogDebug("[Hooks] New active RakClient detected at 0x%08X, registering custom RPCs...", (uintptr_t)pRakClient);
        RegisterCustomRPCs(pRakClient);
    }

    if (data && length > 1)
    {
        unsigned char packetId = data[0];
        unsigned char rpcId = 0;

        if (packetId == 0x14 || packetId == 0x20) // ID_RPC (0x14 in SA-MP, 0x20 standard)
        {
            rpcId = data[1];
        }
        else if (packetId == 0x28 && length > 6 && (data[5] == 0x14 || data[5] == 0x20)) // ID_TIMESTAMP
        {
            rpcId = data[6];
        }

        if (rpcId >= 200 && rpcId <= 204)
        {
            LogDebug("[Hooks] HandleRPCPacket: Intercepted custom RPC %d (length %d bytes, packetId 0x%02X)", rpcId, length, packetId);
        }
    }

    // Unhook temporarily to call original
    DWORD oldProtect;
    uintptr_t hookAddr = g_dwSampBase + SAMP_R2_OFFSET_HANDLE_RPC;
    VirtualProtect((void*)hookAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void*)hookAddr, g_origHandleRpcBytes, 5);

    bool result = ((HandleRPCPacket_t)hookAddr)(pRakClient, data, length, playerId_addr, playerId_port);

    // Re-apply hook jump
    uintptr_t target = (uintptr_t)HookedHandleRPCPacket;
    unsigned char jmp[5] = { 0xE9, 0, 0, 0, 0 };
    *(int*)(jmp + 1) = (int)(target - hookAddr - 5);
    memcpy((void*)hookAddr, jmp, 5);
    VirtualProtect((void*)hookAddr, 5, oldProtect, &oldProtect);

    return result;
}

// -------------------------------------------------------------
// Engine Trampolines & Utilities
// -------------------------------------------------------------

static void* CreateTrampoline(uintptr_t targetAddr, size_t numBytes) {
    unsigned char* tramp = (unsigned char*)VirtualAlloc(nullptr, 32, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
    if (!tramp) return nullptr;
    memcpy(tramp, (void*)targetAddr, numBytes);
    tramp[numBytes] = 0xE9; // JMP rel32
    uintptr_t jmpTarget = targetAddr + numBytes;
    uintptr_t jmpFrom = (uintptr_t)(tramp + numBytes + 5);
    *(int*)(tramp + numBytes + 1) = (int)(jmpTarget - jmpFrom);
    FlushInstructionCache(GetCurrentProcess(), tramp, 32);
    return tramp;
}

static void PlaceJmp(uintptr_t from, uintptr_t to, size_t numBytes) {
    DWORD oldProtect;
    VirtualProtect((void*)from, numBytes, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset((void*)from, 0x90, numBytes);
    unsigned char* p = (unsigned char*)from;
    p[0] = 0xE9;
    *(int*)(p + 1) = (int)(to - from - 5);
    VirtualProtect((void*)from, numBytes, oldProtect, &oldProtect);
    FlushInstructionCache(GetCurrentProcess(), (void*)from, numBytes);
}

// -------------------------------------------------------------
// RenderWare & CClothesBuilder Crash-Prevention Hooks
// -------------------------------------------------------------

typedef void* (*RwTextureCallBack_t)(void* texture, void* pData);
typedef void* (__cdecl *RwTexDictionaryForAllTextures_t)(void* dict, RwTextureCallBack_t fpCallBack, void* pData);
typedef void* (__cdecl *RwTexDictionaryFindNamedTexture_t)(void* dict, const char* name);
typedef void* (__cdecl *GetFirstTexture_t)(void* dict);
typedef void* (__cdecl *BlendTextures1_t)(void* t1);
typedef void* (__cdecl *BlendTextures2_t)(void* t1, void* t2);
typedef void  (__cdecl *RemoveTxdSlot_t)(int slot);
typedef int   (__cdecl *LoadTextureFromImg_t)(unsigned int key);
typedef void  (__cdecl *FlushChannels_t)();
typedef void  (__cdecl *LoadAllRequestedModels_t)(int b);

static BlendTextures1_t g_trampBlend1 = nullptr;
static BlendTextures2_t g_trampBlend2 = nullptr;

// 1. Hook GetFirstTexture (0x00734940)
// Safe: checks for null dict and directly returns first texture from dict->textures
static void* __cdecl HookedGetFirstTexture(void* dict) {
    if (!dict) return nullptr;
    uintptr_t pHead = (uintptr_t)dict + 8;
    uintptr_t pCur = *(uintptr_t*)pHead;
    if (pCur && pCur != pHead) {
        return (void*)(pCur - 8);
    }
    return nullptr;
}

// 2. Hook RwTexDictionaryForAllTextures (0x007F3730)
// Safe: checks for null dict before traversing textures
static void* __cdecl HookedRwTexDictionaryForAllTextures(void* dict, RwTextureCallBack_t fpCallBack, void* pData) {
    if (!dict) return nullptr;
    uintptr_t pHead = (uintptr_t)dict + 8;
    uintptr_t pCur = *(uintptr_t*)pHead;
    while (pCur && pCur != pHead) {
        uintptr_t pNext = *(uintptr_t*)pCur;
        void* pTex = (void*)(pCur - 8);
        if (fpCallBack && !fpCallBack(pTex, pData)) {
            break;
        }
        pCur = pNext;
    }
    return dict;
}

// 3. Hook RwTexDictionaryFindNamedTexture (0x007F39F0)
// Safe: checks for null dict and null name
static void* __cdecl HookedRwTexDictionaryFindNamedTexture(void* dict, const char* name) {
    if (!dict || !name) return nullptr;
    uintptr_t pHead = (uintptr_t)dict + 8;
    uintptr_t pCur = *(uintptr_t*)pHead;
    while (pCur && pCur != pHead) {
        uintptr_t pNext = *(uintptr_t*)pCur;
        char* texName = (char*)(pCur - 8 + 0x10); // RwTexture::name is char[32] at +0x10
        if (_stricmp(texName, name) == 0) {
            return (void*)(pCur - 8);
        }
        pCur = pNext;
    }
    return nullptr;
}

// 4. Hook BlendTextures1 (0x005A5730)
// Safe: checks for null texture before raster dereference
static void* __cdecl HookedBlendTextures1(void* t1) {
    if (!t1) return nullptr;
    if (g_trampBlend1) {
        return g_trampBlend1(t1);
    }
    return t1;
}

// 5. Hook BlendTextures2 (0x005A57B0)
// Safe: handles null textures gracefully
static void* __cdecl HookedBlendTextures2(void* t1, void* t2) {
    if (!t1 && !t2) return nullptr;
    if (!t1) return t2;
    if (!t2) return t1;
    if (g_trampBlend2) {
        return g_trampBlend2(t1, t2);
    }
    return t1;
}

// 6. Hook CClothesBuilder::BlendTattoo / Step (0x005A5F70)
// Fixes GTA SA engine bug where a removed TXD slot (byteMap < 0) is passed into 0x005A5F70,
// causing null dictionary dereferences and crashes at 0x007F3737 & 0x005A57E5!
static void* __cdecl HookedClothesBuilderStep(void* targetTex, int txdSlot, unsigned int nextTexKey, int* outNextSlot) {
    if (txdSlot == -1) {
        if (nextTexKey) {
            auto LoadTextureFromImg = (LoadTextureFromImg_t)0x005A4220;
            auto FlushChannels = (FlushChannels_t)0x0040E3A0;
            int loaded = LoadTextureFromImg(nextTexKey);
            if (outNextSlot) *outNextSlot = loaded;
            FlushChannels();
        } else {
            if (outNextSlot) *outNextSlot = -1;
        }
        return targetTex;
    }

    auto LoadAllRequestedModels = (LoadAllRequestedModels_t)0x0040EA10;
    LoadAllRequestedModels(1);

    if (nextTexKey) {
        auto LoadTextureFromImg = (LoadTextureFromImg_t)0x005A4220;
        auto FlushChannels = (FlushChannels_t)0x0040E3A0;
        int loaded = LoadTextureFromImg(nextTexKey);
        if (outNextSlot) *outNextSlot = loaded;
        FlushChannels();
    } else {
        if (outNextSlot) *outNextSlot = -1;
    }

    // Safely check if txdSlot is allocated and valid in CTxdStore::ms_pTxdPool
    if (txdSlot >= 0 && txdSlot < 5000) {
        uintptr_t txdPoolPtr = *(uintptr_t*)0x00C8800C;
        if (txdPoolPtr) {
            char* byteMap = *(char**)(txdPoolPtr + 4);
            uintptr_t objects = *(uintptr_t*)txdPoolPtr;
            // Only access dictionary if slot is still allocated (byteMap >= 0)!
            if (byteMap && (byteMap[txdSlot] >= 0) && objects) {
                uintptr_t pTxdDef = objects + txdSlot * 12;
                void* dict = *(void**)pTxdDef;
                if (dict) {
                    void* blendTex = HookedGetFirstTexture(dict);
                    if (blendTex) {
                        if (targetTex) {
                            targetTex = HookedBlendTextures2(targetTex, blendTex);
                        } else {
                            targetTex = HookedBlendTextures1(blendTex);
                        }
                    }
                }
                auto RemoveTxdSlot = (RemoveTxdSlot_t)0x004089A0;
                RemoveTxdSlot(20000 + txdSlot);
            }
        }
    }

    return targetTex;
}

void InstallEngineHooks()
{
    if (g_bEngineHooksInstalled) return;

    LogDebug("[Hooks] Installing engine safety hooks...");

    // Create trampolines for original blend functions
    g_trampBlend1 = (BlendTextures1_t)CreateTrampoline(0x005A5730, 5);
    g_trampBlend2 = (BlendTextures2_t)CreateTrampoline(0x005A57B0, 8);

    // Place hooks in GTA SA engine
    PlaceJmp(0x00734940, (uintptr_t)HookedGetFirstTexture, 5);
    PlaceJmp(0x007F3730, (uintptr_t)HookedRwTexDictionaryForAllTextures, 5);
    PlaceJmp(0x007F39F0, (uintptr_t)HookedRwTexDictionaryFindNamedTexture, 5);
    PlaceJmp(0x005A5730, (uintptr_t)HookedBlendTextures1, 5);
    PlaceJmp(0x005A57B0, (uintptr_t)HookedBlendTextures2, 8);
    PlaceJmp(0x005A5F70, (uintptr_t)HookedClothesBuilderStep, 5);

    g_bEngineHooksInstalled = true;
    LogDebug("[Hooks] All 6 engine crash-prevention hooks installed successfully!");
}

void InstallHooks()
{
    // Make sure engine hooks are installed
    InstallEngineHooks();

    g_hSampDll = GetModuleHandleA("samp.dll");
    if (!g_hSampDll)
    {
        LogDebug("[Hooks] ERROR: samp.dll not found!");
        return;
    }

    g_dwSampBase = (uintptr_t)g_hSampDll;
    LogDebug("[Hooks] samp.dll base address: 0x%08X", g_dwSampBase);

    uintptr_t hookAddr = g_dwSampBase + SAMP_R2_OFFSET_HANDLE_RPC;
    DWORD oldProtect;
    VirtualProtect((void*)hookAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(g_origHandleRpcBytes, (void*)hookAddr, 5);

    uintptr_t target = (uintptr_t)HookedHandleRPCPacket;
    unsigned char jmp[5] = { 0xE9, 0, 0, 0, 0 };
    *(int*)(jmp + 1) = (int)(target - hookAddr - 5);
    memcpy((void*)hookAddr, jmp, 5);
    VirtualProtect((void*)hookAddr, 5, oldProtect, &oldProtect);

    g_bHookInstalled = true;
    LogDebug("[Hooks] HandleRPCPacket hooked at 0x%08X", hookAddr);
}

void RemoveHooks()
{
    if (g_bHookInstalled && g_dwSampBase)
    {
        uintptr_t hookAddr = g_dwSampBase + SAMP_R2_OFFSET_HANDLE_RPC;
        DWORD oldProtect;
        VirtualProtect((void*)hookAddr, 5, PAGE_EXECUTE_READWRITE, &oldProtect);
        memcpy((void*)hookAddr, g_origHandleRpcBytes, 5);
        VirtualProtect((void*)hookAddr, 5, oldProtect, &oldProtect);
        g_bHookInstalled = false;
    }
    LogDebug("[Hooks] Hooks removed.");
}

void WriteMemory(uintptr_t address, void* data, size_t size)
{
    DWORD oldProtect;
    VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy((void*)address, data, size);
    VirtualProtect((void*)address, size, oldProtect, &oldProtect);
}

void ReadMemory(uintptr_t address, void* data, size_t size)
{
    DWORD oldProtect;
    VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memcpy(data, (void*)address, size);
    VirtualProtect((void*)address, size, oldProtect, &oldProtect);
}

void NopMemory(uintptr_t address, size_t size)
{
    DWORD oldProtect;
    VirtualProtect((void*)address, size, PAGE_EXECUTE_READWRITE, &oldProtect);
    memset((void*)address, 0x90, size);
    VirtualProtect((void*)address, size, oldProtect, &oldProtect);
}
