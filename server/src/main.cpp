#include "plugin.h"
#include "clothing_manager.h"
#include "network.h"
#include "natives.h"
#include "callbacks.h"

#if defined(_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#endif

// Define global pointers
amx_Align16_t     amx_Align16 = nullptr;
amx_Align32_t     amx_Align32 = nullptr;
amx_Allot_t       amx_Allot = nullptr;
amx_Exec_t        amx_Exec = nullptr;
amx_FindPublic_t  amx_FindPublic = nullptr;
amx_GetAddr_t     amx_GetAddr = nullptr;
amx_GetString_t   amx_GetString = nullptr;
amx_Push_t        amx_Push = nullptr;
amx_PushString_t  amx_PushString = nullptr;
amx_Register_t    amx_Register = nullptr;
amx_Release_t     amx_Release = nullptr;
amx_SetString_t   amx_SetString = nullptr;
amx_StrLen_t      amx_StrLen = nullptr;

logprintf_t logprintf = nullptr;
std::set<AMX*> pAMXFunctions;
// Secret SA-MP SDK export for RakServer (0xE2)
typedef void* (*GetRakServer_t)();
GetRakServer_t pfn_GetRakServer = nullptr;
void* pRakServer = nullptr;

PLUGIN_EXPORT unsigned int PLUGIN_CALL Supports()
{
    return SUPPORTS_VERSION | SUPPORTS_AMX_NATIVES | SUPPORTS_PROCESS_TICK;
}

PLUGIN_EXPORT bool PLUGIN_CALL Load(void **ppData)
{
    logprintf = (logprintf_t)ppData[PLUGIN_DATA_LOGPRINTF];
    
    void **exports = (void **)ppData[PLUGIN_DATA_AMX_EXPORTS];
    amx_Align16     = (amx_Align16_t)   exports[PLUGIN_AMX_EXPORT_Align16];
    amx_Align32     = (amx_Align32_t)   exports[PLUGIN_AMX_EXPORT_Align32];
    amx_Allot       = (amx_Allot_t)     exports[PLUGIN_AMX_EXPORT_Allot];
    amx_Exec        = (amx_Exec_t)      exports[PLUGIN_AMX_EXPORT_Exec];
    amx_FindPublic  = (amx_FindPublic_t)exports[PLUGIN_AMX_EXPORT_FindPublic];
    amx_GetAddr     = (amx_GetAddr_t)   exports[PLUGIN_AMX_EXPORT_GetAddr];
    amx_GetString   = (amx_GetString_t) exports[PLUGIN_AMX_EXPORT_GetString];
    amx_Push        = (amx_Push_t)      exports[PLUGIN_AMX_EXPORT_Push];
    amx_PushString  = (amx_PushString_t)exports[PLUGIN_AMX_EXPORT_PushString];
    amx_Register    = (amx_Register_t)  exports[PLUGIN_AMX_EXPORT_Register];
    amx_Release     = (amx_Release_t)   exports[PLUGIN_AMX_EXPORT_Release];
    amx_SetString   = (amx_SetString_t) exports[PLUGIN_AMX_EXPORT_SetString];
    amx_StrLen      = (amx_StrLen_t)    exports[PLUGIN_AMX_EXPORT_StrLen];

    pfn_GetRakServer = (GetRakServer_t)ppData[0xE2];

    logprintf("===========================================");
    logprintf(" Custom CJ Clothing Plugin v1.0.0");
    logprintf(" Loading...");

    Network::Initialize();

    logprintf(" Plugin Loaded Successfully!");
    logprintf("===========================================");
    return true;
}

PLUGIN_EXPORT void PLUGIN_CALL Unload()
{
    logprintf(" Custom CJ Clothing Plugin Unloading...");
    ClothingManager::CleanupAll();
    logprintf(" Plugin Unloaded.");
}

PLUGIN_EXPORT int PLUGIN_CALL AmxLoad(AMX *amx)
{
    pAMXFunctions.insert(amx);
    return Natives::Register(amx);
}

PLUGIN_EXPORT int PLUGIN_CALL AmxUnload(AMX *amx)
{
    pAMXFunctions.erase(amx);
    return AMX_ERR_NONE;
}

PLUGIN_EXPORT void PLUGIN_CALL ProcessTick()
{
    // Handle periodic tasks
}
