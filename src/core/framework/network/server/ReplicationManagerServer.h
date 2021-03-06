//
//  ReplicationManagerServer.h
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ReplicationManagerServer__
#define __noctisgames__ReplicationManagerServer__

#include <framework/network/portable/Network.h>

#include <framework/network/portable/ReplicationCommand.h>

#include <map>

class EntityManager;
class OutputMemoryBitStream;
class ReplicationManagerTransmissionData;

class ReplicationManagerServer
{
public:
    ReplicationManagerServer(EntityManager* entityManager);
    
    void replicateCreate(uint32_t inNetworkId, uint16_t inInitialDirtyState);
    void replicateDestroy(uint32_t inNetworkId);
    void setStateDirty(uint32_t inNetworkId, uint16_t inDirtyState);
    void handleCreateAckd(uint32_t inNetworkId);
    void removeFromReplication(uint32_t inNetworkId);
    void write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissinData);
    
private:
    EntityManager* _entityManager;
    
    std::map<uint32_t, ReplicationCommand> _networkIdToReplicationCommand;
    
    uint16_t writeCreateAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint16_t inDirtyState);
    uint16_t writeUpdateAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint16_t inDirtyState);
    uint16_t writeDestroyAction(OutputMemoryBitStream& inOutputStream, uint32_t inNetworkId, uint16_t inDirtyState);
};

#endif /* defined(__noctisgames__ReplicationManagerServer__) */
