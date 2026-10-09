/*
 *  SA-MP Plugin SDK — amx.h (Minimal)
 *  AMX (Abstract Machine eXecutor) interface for Pawn scripting
 *  Based on the standard SA-MP Plugin SDK
 */

#ifndef _AMX_H_
#define _AMX_H_

#pragma once

#include <stdint.h>

#if defined(_WIN32) || defined(WIN32) || defined(__WIN32__)
#include <malloc.h>   // for alloca() on MSVC
#endif

// ============================================================================
//  PLATFORM CONFIGURATION
// ============================================================================
// In SA-MP, all AMX natives and API functions use standard __cdecl.
// __stdcall causes catastrophic 8-byte stack corruption on return!
#define AMX_NATIVE_CALL
#define AMXAPI

// ============================================================================
//  AMX TYPES
// ============================================================================
typedef int32_t     cell;
typedef uint32_t    ucell;
typedef cell        (AMX_NATIVE_CALL *AMX_NATIVE)(struct tagAMX *amx, cell *params);
typedef int         (AMXAPI *AMX_CALLBACK)(struct tagAMX *amx, cell index, cell *result, cell *params);
typedef int         (AMXAPI *AMX_DEBUG)(struct tagAMX *amx);

// ============================================================================
//  AMX HEADER
// ============================================================================
typedef struct tagAMX_HEADER
{
    int32_t   size;           // size of the header
    uint16_t  magic;          // signature
    char      file_version;   // file format version
    char      amx_version;    // required version of the AMX
    int16_t   flags;
    int16_t   defsize;        // size of a definition record
    int32_t   cod;            // initial value of COD
    int32_t   dat;            // initial value of DAT
    int32_t   hea;            // initial value of HEA
    int32_t   stp;            // initial value of STP
    int32_t   cip;            // initial value of CIP
    int32_t   publics;        // offset to public function table
    int32_t   natives;        // offset to native function table
    int32_t   libraries;      // offset to libraries table
    int32_t   pubvars;        // offset to public variables table
    int32_t   tags;           // offset to tags table
    int32_t   nametable;      // offset to name table
} AMX_HEADER;

// ============================================================================
//  AMX STRUCTURE
// ============================================================================
typedef struct tagAMX
{
    unsigned char  *base;
    unsigned char  *data;
    AMX_CALLBACK    callback;
    AMX_DEBUG       debug;
    cell            cip;
    cell            frm;
    cell            hea;
    cell            hlw;
    cell            stk;
    cell            stp;
    int             flags;
    long            usertags[4];
    void           *userdata[4];
    int             error;
    int             paramcount;
    cell            pri;
    cell            alt;
    cell            reset_stk;
    cell            reset_hea;
    cell            sysreq_d;
} AMX;

// ============================================================================
//  AMX NATIVE INFO
// ============================================================================
typedef struct tagAMX_NATIVE_INFO
{
    const char    *name;
    AMX_NATIVE     func;
} AMX_NATIVE_INFO;

// ============================================================================
//  AMX FUNCTION TABLE INDICES
// ============================================================================
enum
{
    PLUGIN_AMX_EXPORT_Align16       = 0,
    PLUGIN_AMX_EXPORT_Align32       = 1,
    PLUGIN_AMX_EXPORT_Align64       = 2,
    PLUGIN_AMX_EXPORT_Allot         = 3,
    PLUGIN_AMX_EXPORT_Callback      = 4,
    PLUGIN_AMX_EXPORT_Cleanup       = 5,
    PLUGIN_AMX_EXPORT_Clone         = 6,
    PLUGIN_AMX_EXPORT_Exec          = 7,
    PLUGIN_AMX_EXPORT_FindNative    = 8,
    PLUGIN_AMX_EXPORT_FindPublic    = 9,
    PLUGIN_AMX_EXPORT_FindPubVar    = 10,
    PLUGIN_AMX_EXPORT_FindTagId     = 11,
    PLUGIN_AMX_EXPORT_Flags         = 12,
    PLUGIN_AMX_EXPORT_GetAddr       = 13,
    PLUGIN_AMX_EXPORT_GetNative     = 14,
    PLUGIN_AMX_EXPORT_GetPublic     = 15,
    PLUGIN_AMX_EXPORT_GetPubVar     = 16,
    PLUGIN_AMX_EXPORT_GetString     = 17,
    PLUGIN_AMX_EXPORT_GetTag        = 18,
    PLUGIN_AMX_EXPORT_GetUserData   = 19,
    PLUGIN_AMX_EXPORT_Init          = 20,
    PLUGIN_AMX_EXPORT_InitJIT       = 21,
    PLUGIN_AMX_EXPORT_MemInfo       = 22,
    PLUGIN_AMX_EXPORT_NameLength    = 23,
    PLUGIN_AMX_EXPORT_NativeInfo    = 24,
    PLUGIN_AMX_EXPORT_NumNatives    = 25,
    PLUGIN_AMX_EXPORT_NumPublics    = 26,
    PLUGIN_AMX_EXPORT_NumPubVars    = 27,
    PLUGIN_AMX_EXPORT_NumTags       = 28,
    PLUGIN_AMX_EXPORT_Push          = 29,
    PLUGIN_AMX_EXPORT_PushArray     = 30,
    PLUGIN_AMX_EXPORT_PushString    = 31,
    PLUGIN_AMX_EXPORT_RaiseError    = 32,
    PLUGIN_AMX_EXPORT_Register      = 33,
    PLUGIN_AMX_EXPORT_Release       = 34,
    PLUGIN_AMX_EXPORT_SetCallback   = 35,
    PLUGIN_AMX_EXPORT_SetDebugHook  = 36,
    PLUGIN_AMX_EXPORT_SetString     = 37,
    PLUGIN_AMX_EXPORT_SetUserData   = 38,
    PLUGIN_AMX_EXPORT_StrLen        = 39,
    PLUGIN_AMX_EXPORT_UTF8Check     = 40,
    PLUGIN_AMX_EXPORT_UTF8Get       = 41,
    PLUGIN_AMX_EXPORT_UTF8Len       = 42,
    PLUGIN_AMX_EXPORT_UTF8Put       = 43
};

// ============================================================================
//  AMX FUNCTION POINTERS (resolved from ppData)
// ============================================================================
typedef uint16_t*   (AMXAPI *amx_Align16_t)(uint16_t *v);
typedef uint32_t*   (AMXAPI *amx_Align32_t)(uint32_t *v);
typedef int         (AMXAPI *amx_Allot_t)(AMX *amx, int cells, cell *amx_addr, cell **phys_addr);
typedef int         (AMXAPI *amx_Exec_t)(AMX *amx, cell *retval, int index);
typedef int         (AMXAPI *amx_FindPublic_t)(AMX *amx, const char *funcname, int *index);
typedef int         (AMXAPI *amx_GetAddr_t)(AMX *amx, cell amx_addr, cell **phys_addr);
typedef int         (AMXAPI *amx_GetString_t)(char *dest, const cell *source, int use_wchar, size_t size);
typedef int         (AMXAPI *amx_Push_t)(AMX *amx, cell value);
typedef int         (AMXAPI *amx_PushString_t)(AMX *amx, cell *amx_addr, cell **phys_addr, const char *string, int pack, int use_wchar);
typedef int         (AMXAPI *amx_Register_t)(AMX *amx, const AMX_NATIVE_INFO *nativelist, int number);
typedef int         (AMXAPI *amx_Release_t)(AMX *amx, cell amx_addr);
typedef int         (AMXAPI *amx_SetString_t)(cell *dest, const char *source, int pack, int use_wchar, size_t size);
typedef int         (AMXAPI *amx_StrLen_t)(const cell *cstring, int *length);

// ============================================================================
//  AMX ERROR CODES
// ============================================================================
#define AMX_ERR_NONE        0
#define AMX_ERR_EXIT        1
#define AMX_ERR_ASSERT      2
#define AMX_ERR_STACKERR    3
#define AMX_ERR_BOUNDS      4
#define AMX_ERR_MEMACCESS   5
#define AMX_ERR_INVINSTR    6
#define AMX_ERR_STACKLOW    7
#define AMX_ERR_HEAPLOW     8
#define AMX_ERR_CALLBACK    9
#define AMX_ERR_NATIVE      10
#define AMX_ERR_DIVIDE      11
#define AMX_ERR_SLEEP       12
#define AMX_ERR_INVSTATE    13
#define AMX_ERR_MEMORY      16
#define AMX_ERR_FORMAT      17
#define AMX_ERR_VERSION     18
#define AMX_ERR_NOTFOUND    19
#define AMX_ERR_INDEX       20
#define AMX_ERR_DEBUG       21
#define AMX_ERR_INIT        22
#define AMX_ERR_USERDATA    23
#define AMX_ERR_INIT_JIT    24
#define AMX_ERR_PARAMS      25
#define AMX_ERR_DOMAIN      26
#define AMX_ERR_GENERAL     27

// ============================================================================
//  HELPER MACROS
// ============================================================================

// Get a string parameter from Pawn
#define amx_StrParam(amx, param, result)                                    \
    do {                                                                     \
        cell *amx_cstr_; int amx_length_;                                    \
        amx_GetAddr(amx, param, &amx_cstr_);                               \
        amx_StrLen(amx_cstr_, &amx_length_);                               \
        if (amx_length_ > 0 &&                                              \
            ((result) = (char *)alloca((amx_length_ + 1) * sizeof(*(result)))) != NULL) \
            amx_GetString((char *)(result), amx_cstr_, sizeof(*(result)) > 1, amx_length_ + 1); \
        else (result) = (char *)"";                                         \
    } while (0)

#endif // _AMX_H_
