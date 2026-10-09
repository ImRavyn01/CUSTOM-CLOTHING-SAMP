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

void InstallHooks()
{
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
