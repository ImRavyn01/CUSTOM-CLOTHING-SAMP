/*
 *  Custom CJ Clothing System — Hooks Header
 *  hooks.h
 */

#ifndef _HOOKS_H_
#define _HOOKS_H_

#include <windows.h>
#include <cstdint>

// Install all hooks (pattern scan samp.dll, resolve pointers)
void InstallHooks();

// Remove all hooks and cleanup
void RemoveHooks();

// Pattern scanning in a loaded module
uintptr_t FindPattern(HMODULE hModule, const char* pattern, const char* mask);

// Memory manipulation utilities
void WriteMemory(uintptr_t address, void* data, size_t size);
void ReadMemory(uintptr_t address, void* data, size_t size);
void NopMemory(uintptr_t address, size_t size);

#endif // _HOOKS_H_
