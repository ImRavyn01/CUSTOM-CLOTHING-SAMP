/*
 *  Custom CJ Clothing System — Client ASI Plugin Entry Point
 *  main.cpp
 */

#include <windows.h>
#include "hooks.h"
#include "samp_structs.h"
#include "clothing_client.h"
#include "rpc_handler.h"

#define SAMP_WAIT_TIMEOUT_MS    30000
#define NETGAME_WAIT_TIMEOUT_MS 60000

static HMODULE g_hThisModule = nullptr;

DWORD WINAPI InitThread(LPVOID lpParam)
{
    InitClothingClient();
    LogDebug("==============================================");
    LogDebug(" Custom CJ Clothing ASI Plugin v1.0.0");
    LogDebug(" SA-MP 0.3.7 R2 | GTA SA 1.0 US");
    LogDebug("==============================================");

    LogDebug("[Init] Waiting for samp.dll to load...");
    HMODULE sampMod = nullptr;
    DWORD startTime = GetTickCount();

    while (!sampMod)
    {
        sampMod = GetModuleHandleA("samp.dll");
        if (!sampMod)
        {
            if (GetTickCount() - startTime > SAMP_WAIT_TIMEOUT_MS)
            {
                LogDebug("[Init] TIMEOUT: samp.dll not found after %d ms.", SAMP_WAIT_TIMEOUT_MS);
                return 0;
            }
            Sleep(100);
        }
    }

    LogDebug("[Init] samp.dll found at 0x%08X", (uintptr_t)sampMod);
    Sleep(1000);

    // Install HandleRPCPacket hook
    LogDebug("[Init] Installing hooks...");
    InstallHooks();

    // Wait for NetGame and RakClient to be allocated
    LogDebug("[Init] Waiting for SA-MP NetGame initialization...");
    startTime = GetTickCount();

    while (true)
    {
        if (g_dwSampBase)
        {
            void* pNetGame = *(void**)(g_dwSampBase + SAMP_R2_OFFSET_NETGAME);
            if (pNetGame)
            {
                void* pRakClient = *(void**)((char*)pNetGame + SAMP_R2_NETGAME_RAKCLIENT);
                if (pRakClient)
                {
                    LogDebug("[Init] NetGame found at 0x%08X, RakClient at 0x%08X",
                             (uintptr_t)pNetGame, (uintptr_t)pRakClient);
                    break;
                }
            }
        }

        if (GetTickCount() - startTime > NETGAME_WAIT_TIMEOUT_MS)
        {
            LogDebug("[Init] TIMEOUT waiting for NetGame after %d ms.", NETGAME_WAIT_TIMEOUT_MS);
            break;
        }

        Sleep(500);
    }

    // Register our RPC handlers in RakClient
    LogDebug("[Init] Registering custom RPC handlers...");
    RegisterCustomRPCs();

    LogDebug("[Init] ==============================================");
    LogDebug("[Init] Initialization complete!");
    LogDebug("[Init] Clothing system is ready.");
    LogDebug("[Init] ==============================================");

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
        case DLL_PROCESS_ATTACH:
        {
            g_hThisModule = hModule;
            DisableThreadLibraryCalls(hModule);

            HANDLE hThread = CreateThread(nullptr, 0, InitThread, nullptr, 0, nullptr);
            if (hThread) CloseHandle(hThread);
            break;
        }
        case DLL_PROCESS_DETACH:
        {
            RemoveHooks();
            break;
        }
    }
    return TRUE;
}
