//
//  NetworkManagerServer.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerServer.h"

#include "Server.h"
#include "ReplicationManagerTransmissionData.h"
#include "StringUtils.h"
#include "World.h"
#include "Timing.h"

NetworkManagerServer* NetworkManagerServer::sInstance;

NetworkManagerServer::NetworkManagerServer() :
m_iNewPlayerId(1),
m_iNewNetworkId(1),
m_fTimeBetweenStatePackets(0.033f),
m_fClientDisconnectTimeout(3.f)
{
}

bool NetworkManagerServer::StaticInit(uint16_t inPort)
{
    sInstance = new NetworkManagerServer();
    return sInstance->Init(inPort);
}

void NetworkManagerServer::HandleConnectionReset(const SocketAddress& inFromAddress)
{
    //just dc the client right away...
    auto it = mAddressToClientMap.find(inFromAddress);
    if (it != mAddressToClientMap.end())
    {
        HandleClientDisconnected(it->second);
    }
}

void NetworkManagerServer::ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
    //try to get the client proxy for this address
    //pass this to the client proxy to process
    auto it = mAddressToClientMap.find(inFromAddress);
    if (it == mAddressToClientMap.end())
    {
        //didn't find one? it's a new cilent..is the a HELO? if so, create a client proxy...
        HandlePacketFromNewClient(inInputStream, inFromAddress);
    }
    else
    {
        ProcessPacket((*it).second, inInputStream);
    }
}

void NetworkManagerServer::ProcessPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream)
{
    //remember we got a packet so we know not to disconnect for a bit
    inClientProxy->UpdateLastPacketTime();
    
    uint32_t	packetType;
    inInputStream.Read(packetType);
    switch(packetType)
    {
        case kHelloCC:
            //need to resend welcome. to be extra safe we should check the name is the one we expect from this address,
            //otherwise something weird is going on...
            SendWelcomePacket(inClientProxy);
            break;
        case kInputCC:
            if (inClientProxy->GetDeliveryNotificationManager().ReadAndProcessState(inInputStream))
            {
                HandleInputPacket(inClientProxy, inInputStream);
            }
            break;
        default:
            LOG("Unknown packet type received from %s", inClientProxy->GetSocketAddress().ToString().c_str());
            break;
    }
}

void NetworkManagerServer::HandlePacketFromNewClient(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
    //read the beginning- is it a hello?
    uint32_t	packetType;
    inInputStream.Read(packetType);
    if (packetType == kHelloCC)
    {
        //read the name
        std::string name;
        inInputStream.Read(name);
        ClientProxyPtr newClientProxy = std::make_shared< ClientProxy >(inFromAddress, name, m_iNewPlayerId++);
        mAddressToClientMap[inFromAddress] = newClientProxy;
        m_iPlayerIdToClientMap[newClientProxy->GetPlayerId()] = newClientProxy;
        
        //tell the server about this client, spawn a cat, etc...
        //if we had a generic message system, this would be a good use for it...
        //instead we'll just tell the server directly
        static_cast<Server*> (Engine::sInstance.get())->HandleNewClient(newClientProxy);
        
        //and welcome the client...
        SendWelcomePacket(newClientProxy);
        
        //and now init the replication manager with everything we know about!
        for (const auto& pair: m_networkIdToGameObjectMap)
        {
            newClientProxy->GetReplicationManagerServer().ReplicateCreate(pair.first, pair.second->GetAllStateMask());
        }
    }
    else
    {
        //bad incoming packet from unknown client- we're under attack!!
        LOG("Bad incoming packet from unknown client at socket %s", inFromAddress.ToString().c_str());
    }
}

void NetworkManagerServer::SendWelcomePacket(ClientProxyPtr inClientProxy)
{
    OutputMemoryBitStream welcomePacket;
    
    welcomePacket.Write(kWelcomeCC);
    welcomePacket.Write(inClientProxy->GetPlayerId());
    
    LOG("Server Welcoming, new client '%s' as player %d", inClientProxy->GetName().c_str(), inClientProxy->GetPlayerId());
    
    SendPacket(welcomePacket, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::RespawnCats()
{
    for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
    {
        ClientProxyPtr clientProxy = it->second;
        
        clientProxy->RespawnCatIfNecessary();
    }
}

void NetworkManagerServer::SendOutgoingPackets()
{
    //let's send a client a state packet whenever their move has come in...
    for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
    {
        ClientProxyPtr clientProxy = it->second;
        //process any timed out packets while we're going through the list
        clientProxy->GetDeliveryNotificationManager().ProcessTimedOutPackets();
        
        if (clientProxy->IsLastMoveTimestampDirty())
        {
            SendStatePacketToClient(clientProxy);
        }
    }
}

void NetworkManagerServer::UpdateAllClients()
{
    for (auto it = mAddressToClientMap.begin(), end = mAddressToClientMap.end(); it != end; ++it)
    {
        //process any timed out packets while we're going throug hthe list
        it->second->GetDeliveryNotificationManager().ProcessTimedOutPackets();
        
        SendStatePacketToClient(it->second);
    }
}

void NetworkManagerServer::SendStatePacketToClient(ClientProxyPtr inClientProxy)
{
    //build state packet
    OutputMemoryBitStream	statePacket;
    
    //it's state!
    statePacket.Write(kStateCC);
    
    InFlightPacket* ifp = inClientProxy->GetDeliveryNotificationManager().WriteState(statePacket);
    
    WriteLastMoveTimestampIfDirty(statePacket, inClientProxy);
    
    ReplicationManagerTransmissionData* rmtd = new ReplicationManagerTransmissionData(&inClientProxy->GetReplicationManagerServer());
    inClientProxy->GetReplicationManagerServer().Write(statePacket, rmtd);
    ifp->SetTransmissionData('RPLM', TransmissionDataPtr(rmtd));
    
    SendPacket(statePacket, inClientProxy->GetSocketAddress());
}

void NetworkManagerServer::WriteLastMoveTimestampIfDirty(OutputMemoryBitStream& inOutputStream, ClientProxyPtr inClientProxy)
{
    //first, dirty?
    bool isTimestampDirty = inClientProxy->IsLastMoveTimestampDirty();
    inOutputStream.Write(isTimestampDirty);
    if (isTimestampDirty)
    {
        inOutputStream.Write(inClientProxy->GetUnprocessedMoveList().GetLastMoveTimestamp());
        inClientProxy->SetIsLastMoveTimestampDirty(false);
    }
}

//should we ask the server for this? or run through the world ourselves?
void NetworkManagerServer::AddWorldStateToPacket(OutputMemoryBitStream& inOutputStream)
{
    const auto& gameObjects = World::sInstance->GetGameObjects();
    
    //now start writing objects- do we need to remember how many there are? we can check first...
    inOutputStream.Write(gameObjects.size());
    
    for (GameObjectPtr gameObject : gameObjects)
    {
        inOutputStream.Write(gameObject->GetNetworkId());
        inOutputStream.Write(gameObject->GetClassId());
        gameObject->Write(inOutputStream, 0xffffffff);
    }
}

int NetworkManagerServer::GetNewNetworkId()
{
    int toRet = m_iNewNetworkId++;
    if (m_iNewNetworkId < toRet)
    {
        LOG("Network ID Wrap Around!!! You've been playing way too long...", 0);
    }
    
    return toRet;
}

void NetworkManagerServer::HandleInputPacket(ClientProxyPtr inClientProxy, InputMemoryBitStream& inInputStream)
{
    uint32_t moveCount = 0;
    Move move;
    inInputStream.Read(moveCount, 2);
    
    for (; moveCount > 0; --moveCount)
    {
        if (move.Read(inInputStream))
        {
            if (inClientProxy->GetUnprocessedMoveList().AddMoveIfNew(move))
            {
                inClientProxy->SetIsLastMoveTimestampDirty(true);
            }
        }
    }
}

ClientProxyPtr NetworkManagerServer::GetClientProxy(int inPlayerId) const
{
    auto it = m_iPlayerIdToClientMap.find(inPlayerId);
    if (it != m_iPlayerIdToClientMap.end())
    {
        return it->second;
    }
    
    return nullptr;
}

void NetworkManagerServer::CheckForDisconnects()
{
    std::vector<ClientProxyPtr> clientsToDC;
    
    float minAllowedLastPacketFromClientTime = Timing::sInstance.GetTime() - m_fClientDisconnectTimeout;
    for (const auto& pair: mAddressToClientMap)
    {
        if (pair.second->GetLastPacketFromClientTime() < minAllowedLastPacketFromClientTime)
        {
            //can't remove from map while in iterator, so just remember for later...
            clientsToDC.push_back(pair.second);
        }
    }
    
    for (ClientProxyPtr client: clientsToDC)
    {
        HandleClientDisconnected(client);
    }
}

void NetworkManagerServer::HandleClientDisconnected(ClientProxyPtr inClientProxy)
{
    m_iPlayerIdToClientMap.erase(inClientProxy->GetPlayerId());
    mAddressToClientMap.erase(inClientProxy->GetSocketAddress());
    static_cast<Server*> (Engine::sInstance.get())->HandleLostClient(inClientProxy);
    
    //was that the last client? if so, bye!
    if (mAddressToClientMap.empty())
    {
        m_iNewPlayerId = 1;
        // Engine::sInstance->SetShouldKeepRunning(false);
    }
}

void NetworkManagerServer::RegisterGameObject(GameObjectPtr inGameObject)
{
    //assign network id
    int newNetworkId = GetNewNetworkId();
    inGameObject->SetNetworkId(newNetworkId);
    
    //add mapping from network id to game object
    m_networkIdToGameObjectMap[newNetworkId] = inGameObject;
    
    //tell all client proxies this is new...
    for (const auto& pair: mAddressToClientMap)
    {
        pair.second->GetReplicationManagerServer().ReplicateCreate(newNetworkId, inGameObject->GetAllStateMask());
    }
}

void NetworkManagerServer::UnregisterGameObject(GameObject* inGameObject)
{
    int networkId = inGameObject->GetNetworkId();
    m_networkIdToGameObjectMap.erase(networkId);
    
    //tell all client proxies to STOP replicating!
    //tell all client proxies this is new...
    for (const auto& pair: mAddressToClientMap)
    {
        pair.second->GetReplicationManagerServer().ReplicateDestroy(networkId);
    }
}

void NetworkManagerServer::SetStateDirty(int inNetworkId, uint32_t inDirtyState)
{
    //tell everybody this is dirty
    for (const auto& pair: mAddressToClientMap)
    {
        pair.second->GetReplicationManagerServer().SetStateDirty(inNetworkId, inDirtyState);
    }
}