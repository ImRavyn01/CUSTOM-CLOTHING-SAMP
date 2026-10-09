import codecs
with codecs.open('temp_network.cpp', 'r', encoding='utf-16') as f:
    data = f.read()
import re
pattern = re.compile(r'void Network::SendRPC.*?\#endif\n}', re.DOTALL)
replacement = r'''void Network::SendRPC(int rpcId, ClothingBitStream* bitStream,
                      int priority, int reliability,
                      char orderingChannel, unsigned short playerId,
                      bool broadcast, bool shiftTimestamp)
{
    EnsureNetworkReady();
    if (!pRakServer) return;

    void** vtable = *(void***)pRakServer;
    if (!vtable) return;

    // Gunakan fungsi aslinya dengan YSF signature
    typedef bool (__thiscall *RakServer_RPC_t)(void* _this, int* uniqueID, RakNet::BitStream* bitStream, PacketPriority priority, PacketReliability reliability, char orderingChannel, PlayerID playerId, bool broadcast, bool shiftTimestamp);
    RakServer_RPC_t rpcFunc = (RakServer_RPC_t)vtable[32];
    if (!rpcFunc) return;

    PlayerID targetPlayer;
    targetPlayer.binaryAddress = 0xFFFFFFFF;
    targetPlayer.port = 0xFFFF;

    static int s_rpcId[256];
    s_rpcId[rpcId & 0xFF] = rpcId;
    int* pRpcId = &s_rpcId[rpcId & 0xFF];

    logprintf("[Clothing] SendRPC (C++): id=%d vtable[32]=0x%08X", rpcId, (uintptr_t)rpcFunc);

    RakNet::BitStream* pRakBitStream = (RakNet::BitStream*)bitStream;

#if defined(_WIN32)
    __try {
        rpcFunc(pRakServer, pRpcId, pRakBitStream, (PacketPriority)priority, (PacketReliability)reliability, orderingChannel, targetPlayer, true, shiftTimestamp);
        logprintf("[Clothing] SendRPC (C++): Success");
    }
    __except(EXCEPTION_EXECUTE_HANDLER) {
        logprintf("[Clothing] SendRPC (C++): CRASH INTERCEPTED!");
    }
#endif
}'''
new_data = pattern.sub(replacement, data)
with codecs.open('D:\\Rahmat\\PROJECT\\CUSTOM CLOTHING CJ SAMP\\PLUGIN SOURCE CODE\\server\\src\\network.cpp', 'w', encoding='utf-8') as f:
    f.write(new_data)
