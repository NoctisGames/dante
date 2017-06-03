//
//  ReplicationManagerServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerServer__
#define __noctisgames__ReplicationManagerServer__

#include "Network.h"

#include "ReplicationCommand.h"

#include <unordered_map>

class OutputMemoryBitStream;
class ReplicationManagerTransmissionData;

class ReplicationManagerServer
{
public:
    void replicateCreate(int inNetworkId, uint32_t inInitialDirtyState);
    
    void replicateDestroy(int inNetworkId);
    
    void setStateDirty(int inNetworkId, uint32_t inDirtyState);
    
    void handleCreateAckd(int inNetworkId);
    
    void removeFromReplication(int inNetworkId);
    
    void write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissinData);
    
private:
    std::unordered_map<int, ReplicationCommand> m_networkIdToReplicationCommand;
    
    uint32_t writeCreateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);
    
    uint32_t writeUpdateAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);
    
    uint32_t writeDestroyAction(OutputMemoryBitStream& inOutputStream, int inNetworkId, uint32_t inDirtyState);
};

#endif /* defined(__noctisgames__ReplicationManagerServer__) */
