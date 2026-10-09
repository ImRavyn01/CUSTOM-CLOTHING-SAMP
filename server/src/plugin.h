#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <set>
#include "../../sdk/plugincommon.h"
#include "../../sdk/amx/amx.h"

// AMX function pointers
extern amx_Align16_t     amx_Align16;
extern amx_Align32_t     amx_Align32;
extern amx_Allot_t       amx_Allot;
extern amx_Exec_t        amx_Exec;
extern amx_FindPublic_t  amx_FindPublic;
extern amx_GetAddr_t     amx_GetAddr;
extern amx_GetString_t   amx_GetString;
extern amx_Push_t        amx_Push;
extern amx_PushString_t  amx_PushString;
extern amx_Register_t    amx_Register;
extern amx_Release_t     amx_Release;
extern amx_SetString_t   amx_SetString;
extern amx_StrLen_t      amx_StrLen;

// Logging function
extern logprintf_t logprintf;

// Active AMX scripts
extern std::set<AMX*> pAMXFunctions;

// RakServer base pointer
extern void* pRakServer;
typedef void* (*GetRakServer_t)();
extern GetRakServer_t pfn_GetRakServer;

#endif // _PLUGIN_H_
