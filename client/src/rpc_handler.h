#ifndef _RPC_HANDLER_H_
#define _RPC_HANDLER_H_

#include "samp_structs.h"

void RegisterCustomRPCs(void* pExplicitRakClient = nullptr);

// RPC Handlers
void __cdecl RPC_ClothingSet(RPCParameters* rpcParams);
void __cdecl RPC_ClothingRemove(RPCParameters* rpcParams);
void __cdecl RPC_ClothingRemoveAll(RPCParameters* rpcParams);
void __cdecl RPC_ClothingSync(RPCParameters* rpcParams);
void __cdecl RPC_ClothingCustomAsset(RPCParameters* rpcParams);

#endif // _RPC_HANDLER_H_
