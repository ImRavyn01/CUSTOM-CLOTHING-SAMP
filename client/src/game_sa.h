#ifndef _GAME_SA_H_
#define _GAME_SA_H_

#include <cstdint>
#include <windows.h>

// GTA SA 1.0 US addresses (verified from plugin-sdk)
#define FUNC_CPedClothesDesc_Initialise             0x5A78F0
#define FUNC_CPedClothesDesc_SetModel1              0x5A7910
#define FUNC_CPedClothesDesc_SetModel2              0x5A7920
#define FUNC_CPedClothesDesc_SetTextureAndModel_Int 0x5A8050
#define FUNC_CPedClothesDesc_SetTextureAndModel_Str 0x5A8080
#define FUNC_CClothes_RebuildPlayer                 0x5A82C0
#define FUNC_FindPlayerPed                          0x56E210

class CPedClothesDesc {
public:
    unsigned int m_anModelKeys[10];
    unsigned int m_anTextureKeys[18];
    float        m_fFatStat;
    float        m_fMuscleStat;

    void Initialise() {
        ((void(__thiscall*)(CPedClothesDesc*))FUNC_CPedClothesDesc_Initialise)(this);
    }

    void SetTextureAndModel(const char* texture, const char* model, int bodypart) {
        ((void(__thiscall*)(CPedClothesDesc*, const char*, const char*, int))FUNC_CPedClothesDesc_SetTextureAndModel_Str)(this, texture, model, bodypart);
    }

    void SetTextureAndModel(unsigned int texture, unsigned int model, int bodypart) {
        ((void(__thiscall*)(CPedClothesDesc*, unsigned int, unsigned int, int))FUNC_CPedClothesDesc_SetTextureAndModel_Int)(this, texture, model, bodypart);
    }
};

class CPed {
};

class CPlayerPed : public CPed {
};

class CClothes {
public:
    static void RebuildPlayer(CPed* ped, bool bIgnoreFatAndMuscle = true) {
        if (!ped) return;

        // Reset CClothes::ms_clothesDesc (0xBC1C78) so GTA SA does not skip rebuilding
        memset((void*)0xBC1C78, 0, 0x78);

        // SA-MP 0.3.7 patches 0x5A82C0 with 0xC3 (ret) to disable singleplayer clothing rebuilds.
        // We temporarily unpatch it by writing the original GTA SA 1.0 US opcode (0x56 = push esi),
        // call the native function, and restore 0xC3 for complete isolation and stability.
        DWORD oldProtect;
        VirtualProtect((void*)FUNC_CClothes_RebuildPlayer, 1, PAGE_EXECUTE_READWRITE, &oldProtect);
        *(unsigned char*)FUNC_CClothes_RebuildPlayer = 0x56;

        ((void(__cdecl*)(CPed*, bool))FUNC_CClothes_RebuildPlayer)(ped, bIgnoreFatAndMuscle);

        *(unsigned char*)FUNC_CClothes_RebuildPlayer = 0xC3;
        VirtualProtect((void*)FUNC_CClothes_RebuildPlayer, 1, oldProtect, &oldProtect);
    }
};

inline CPlayerPed* FindPlayerPed(int playerId = -1) {
    return ((CPlayerPed*(__cdecl*)(int))FUNC_FindPlayerPed)(playerId);
}

inline bool IsValidPedPointer(void* ptr) {
    if (!ptr) return false;
    uintptr_t addr = (uintptr_t)ptr;
    if (addr < 0x10000 || addr >= 0x7FFE0000) return false;
    return (IsBadReadPtr(ptr, 4) == FALSE);
}

inline bool IsValidPed(CPed* pPed) {
    if (!IsValidPedPointer(pPed)) return false;
    if (IsBadReadPtr(pPed, 0x590)) return false;

    // Check model index: clothing is only valid for CJ (Skin 0)
    short modelIndex = *(short*)((char*)pPed + 0x22);
    if (modelIndex != 0) return false;

    // Check RwClump (offset 0x18)
    void* pRwClump = *(void**)((char*)pPed + 0x18);
    if (!IsValidPedPointer(pRwClump)) return false;

    // Check AnimBlendAssoc (offset 0x47c)
    void* pAnimBlend = *(void**)((char*)pPed + 0x47c);
    if (!IsValidPedPointer(pAnimBlend)) return false;

    // Check PlayerData (offset 0x480)
    void* pPlayerData = *(void**)((char*)pPed + 0x480);
    if (!IsValidPedPointer(pPlayerData)) return false;
    if (IsBadReadPtr(pPlayerData, 8)) return false;

    // Check ClothesDesc (offset 0x04 in PlayerData)
    void* pClothesDesc = *(void**)((char*)pPlayerData + 0x04);
    if (!IsValidPedPointer(pClothesDesc)) return false;
    if (IsBadReadPtr(pClothesDesc, 0x78)) return false;

    return true;
}

inline CPedClothesDesc* GetPedClothesDesc(CPed* pPed) {
    if (!IsValidPed(pPed)) return nullptr;
    void* pPlayerData = *(void**)((char*)pPed + 0x480);
    return *(CPedClothesDesc**)((char*)pPlayerData + 0x04);
}

#endif // _GAME_SA_H_
