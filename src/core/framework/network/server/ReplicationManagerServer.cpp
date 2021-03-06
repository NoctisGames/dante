//
//  ReplicationManagerServer.cpp
//  noctisgames
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <framework/network/server/ReplicationManagerServer.h>

#include <framework/entity/EntityManager.h>
#include <framework/network/portable/OutputMemoryBitStream.h>
#include <framework/network/server/ReplicationManagerTransmissionData.h>

#include <framework/entity/EntityManager.h>
#include <framework/network/portable/ReplicationAction.h>
#include <framework/util/macros.h>
#include <framework/entity/Entity.h>
#include <framework/entity/EntityNetworkController.h>

ReplicationManagerServer::ReplicationManagerServer(EntityManager* entityManager) : _entityManager(entityManager)
{
    // Empty
}

void ReplicationManagerServer::replicateCreate(uint32_t inNetworkId, uint16_t inInitialDirtyState)
{
    _networkIdToReplicationCommand[inNetworkId] = ReplicationCommand(inInitialDirtyState);
}

void ReplicationManagerServer::replicateDestroy(uint32_t inNetworkId)
{
    //it's broken if we don't find it...
    _networkIdToReplicationCommand[inNetworkId].setDestroy();
}

void ReplicationManagerServer::removeFromReplication(uint32_t inNetworkId)
{
    _networkIdToReplicationCommand.erase(inNetworkId);
}

void ReplicationManagerServer::setStateDirty(uint32_t inNetworkId, uint16_t inDirtyState)
{
    _networkIdToReplicationCommand[inNetworkId].addDirtyState(inDirtyState);
}

void ReplicationManagerServer::handleCreateAckd(uint32_t inNetworkId)
{
    _networkIdToReplicationCommand[inNetworkId].handleCreateAckd();
}

void ReplicationManagerServer::write(OutputMemoryBitStream& inOutputStream, ReplicationManagerTransmissionData* ioTransmissinData)
{
    //run through each replication command and do something...
    for (auto& pair: _networkIdToReplicationCommand)
    {
        ReplicationCommand& replicationCommand = pair.second;
        if (replicationCommand.hasDirtyState())
        {
            uint32_t networkId = pair.first;
            
            //well, first write the network id...
            inOutputStream.write(networkId);
            
            //only need 2 bits for action...
            ReplicationAction action = replicationCommand.getAction();
            inOutputStream.write<uint8_t, 2>(static_cast<uint8_t>(action));
            
            uint16_t writtenState = 0;
            uint16_t dirtyState = replicationCommand.getDirtyState();
            
            //now do what?
            switch(action)
            {
                case RA_Create:
                    writtenState = writeCreateAction(inOutputStream, networkId, dirtyState);
                    break;
                case RA_Update:
                    writtenState = writeUpdateAction(inOutputStream, networkId, dirtyState);
                    break;
                case RA_Destroy:
                    //don't need anything other than state!
                    writtenState = writeDestroyAction(inOutputStream, networkId, dirtyState);
                    break;
            }
            
            ioTransmissinData->addTransmission(networkId, action, writtenState);
            
            //let's pretend everything was written- don't make this too hard
            replicationCommand.clearDirtyState(writtenState);
        }
    }
}

uint16_t ReplicationManagerServer::writeCreateAction(OutputMemoryBitStream& op, uint32_t networkID, uint16_t dirtyState)
{
    //need object
    Entity* e = _entityManager->getEntityByID(networkID);
    //need 4 cc
    op.write(e->getEntityDef().type);
    
    return e->getNetworkController()->write(op, dirtyState);
}

uint16_t ReplicationManagerServer::writeUpdateAction(OutputMemoryBitStream& op, uint32_t networkID, uint16_t dirtyState)
{
    //need object
    Entity* e = _entityManager->getEntityByID(networkID);
    
    //if we can't find the entity on the other side, we won't be able to read the written data (since we won't know which class wrote it)
    //so we need to know how many bytes to skip.
    
    //this means we need byte sand each new object needs to be byte aligned
    
    return e->getNetworkController()->write(op, dirtyState);
}

uint16_t ReplicationManagerServer::writeDestroyAction(OutputMemoryBitStream& op, uint32_t networkID, uint16_t dirtyState)
{
    UNUSED(op);
    UNUSED(networkID);
    UNUSED(dirtyState);
    
    //don't have to do anything- action already written
    
    return dirtyState;
}
