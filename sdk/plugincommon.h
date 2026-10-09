/*
 *  SA-MP Plugin SDK — plugincommon.h (Minimal)
 *  Based on the standard SA-MP Plugin SDK by maddinat0r
 *  https://github.com/maddinat0r/samp-plugin-sdk
 */

#ifndef _PLUGINCOMMON_H_
#define _PLUGINCOMMON_H_

#pragma once

// Plugin data indices (passed via ppData in Load)
#define PLUGIN_DATA_LOGPRINTF      0x00
#define PLUGIN_DATA_AMX_EXPORTS    0x10
#define PLUGIN_DATA_CALLPUBLIC_FS  0x11
#define PLUGIN_DATA_CALLPUBLIC_GM  0x12

// Plugin supports flags (returned by Supports)
#define SUPPORTS_VERSION           0x0200
#define SUPPORTS_AMX_NATIVES       0x010000
#define SUPPORTS_PROCESS_TICK      0x020000

// Plugin export function IDs
enum PLUGIN_EXPORTS
{
    PLUGIN_EXPORT_Supports = 0,
    PLUGIN_EXPORT_Load     = 1,
    PLUGIN_EXPORT_Unload   = 2,
    PLUGIN_EXPORT_AmxLoad  = 3,
    PLUGIN_EXPORT_AmxUnload = 4,
    PLUGIN_EXPORT_ProcessTick = 5
};

// Export macros
#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
    #define PLUGIN_EXPORT extern "C" __declspec(dllexport)
    #define PLUGIN_CALL __stdcall
#else
    #define PLUGIN_EXPORT extern "C" __attribute__((visibility("default")))
    #define PLUGIN_CALL
#endif

typedef void (*logprintf_t)(const char* format, ...);

#endif // _PLUGINCOMMON_H_
