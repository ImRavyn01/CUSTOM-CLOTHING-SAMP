#include "rpc_handler.h"
#include "clothing_client.h"
#include "samp_structs.h"
#include "../../shared/clothing_common.h"

void RegisterCustomRPCs(void* pExplicitRakClient) {
    void* pRakClient = pExplicitRakClient;
    if (!pRakClient && g_dwSampBase) {
        void* pNetGame = *(void**)(g_dwSampBase + SAMP_R2_OFFSET_NETGAME);
        if (pNetGame) {
            pRakClient = *(void**)((char*)pNetGame + SAMP_R2_NETGAME_RAKCLIENT);
        }
    }

    if (!pRakClient) {
        LogDebug("[RPC] ERROR: pRakClient is null!");
        return;
    }

    LogDebug("[RPC] Found pRakClient at 0x%08X", (uintptr_t)pRakClient);

    // Call RakClient::RegisterAsRemoteProcedureCall at samp.dll + 0x34BE0
    // Signature: void __thiscall RegisterAsRemoteProcedureCall(int* uniqueID, void (*functionPointer)(RPCParameters* rpcParams))
    typedef void (__thiscall *RegisterRPC_t)(void* pRakClient, int* uniqueID, RPCFunction handler);
    RegisterRPC_t pfnRegisterRPC = (RegisterRPC_t)(g_dwSampBase + 0x34BE0);

    int rpcIdSet = RPC_CLOTHING_SET;
    pfnRegisterRPC(pRakClient, &rpcIdSet, RPC_ClothingSet);

    int rpcIdRemove = RPC_CLOTHING_REMOVE;
    pfnRegisterRPC(pRakClient, &rpcIdRemove, RPC_ClothingRemove);

    int rpcIdRemoveAll = RPC_CLOTHING_REMOVE_ALL;
    pfnRegisterRPC(pRakClient, &rpcIdRemoveAll, RPC_ClothingRemoveAll);

    int rpcIdSync = RPC_CLOTHING_SYNC;
    pfnRegisterRPC(pRakClient, &rpcIdSync, RPC_ClothingSync);

    int rpcIdCustomAsset = RPC_CLOTHING_CUSTOM_ASSET;
    pfnRegisterRPC(pRakClient, &rpcIdCustomAsset, RPC_ClothingCustomAsset);

    LogDebug("[RPC] Custom RPCs (200-204) registered via RegisterAsRemoteProcedureCall at 0x%08X!", (uintptr_t)pfnRegisterRPC);
}

void __cdecl RPC_ClothingSet(RPCParameters* rpcParams) {
    LogDebug("[RPC] RPC_ClothingSet called! bits=%d", rpcParams->numberOfBitsOfData);
    BitStream bs(rpcParams->input, rpcParams->numberOfBitsOfData);
    ClothingPacketSet packet;
    memset(&packet, 0, sizeof(packet));
    bs.ReadBytes(&packet, sizeof(packet));
    
    ApplyClothing(packet.playerId, packet.slot.bodyPart, packet.slot.textureName, packet.slot.modelName, packet.slot.customModelId);
}

void __cdecl RPC_ClothingRemove(RPCParameters* rpcParams) {
    LogDebug("[RPC] RPC_ClothingRemove called! bits=%d", rpcParams->numberOfBitsOfData);
    BitStream bs(rpcParams->input, rpcParams->numberOfBitsOfData);
    ClothingPacketRemove packet;
    memset(&packet, 0, sizeof(packet));
    bs.ReadBytes(&packet, sizeof(packet));
    
    RemoveClothing(packet.playerId, packet.bodyPart);
}

void __cdecl RPC_ClothingRemoveAll(RPCParameters* rpcParams) {
    LogDebug("[RPC] RPC_ClothingRemoveAll called! bits=%d", rpcParams->numberOfBitsOfData);
    BitStream bs(rpcParams->input, rpcParams->numberOfBitsOfData);
    ClothingPacketRemoveAll packet;
    memset(&packet, 0, sizeof(packet));
    bs.ReadBytes(&packet, sizeof(packet));
    
    RemoveAllClothing(packet.playerId);
}

void __cdecl RPC_ClothingSync(RPCParameters* rpcParams) {
    LogDebug("[RPC] RPC_ClothingSync called! bits=%d", rpcParams->numberOfBitsOfData);
    BitStream bs(rpcParams->input, rpcParams->numberOfBitsOfData);
    ClothingPacketSync packet;
    memset(&packet, 0, sizeof(packet));
    bs.ReadBytes(&packet, sizeof(packet));
    
    SyncAllClothing(packet.playerId, packet.slots, packet.slotCount);
}

void __cdecl RPC_ClothingCustomAsset(RPCParameters* rpcParams) {
    LogDebug("[RPC] RPC_ClothingCustomAsset called! bits=%d", rpcParams->numberOfBitsOfData);
    BitStream bs(rpcParams->input, rpcParams->numberOfBitsOfData);
    ClothingPacketCustomAsset packet;
    memset(&packet, 0, sizeof(packet));
    bs.ReadBytes(&packet, sizeof(packet));
    
    RegisterCustomAsset(packet.assetId, packet.baseId, packet.dffName, packet.txdName);
}
