//
//  NetworkManagerClient.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "NetworkManagerClient.h"

#include "InputManager.h"
#include "StringUtils.h"
#include "GameObjectRegistry.h"
#include "Timing.h"

NetworkManagerClient* NetworkManagerClient::sInstance;

namespace
{
    const float kTimeBetweenHellos = 1.f;
    const float kTimeBetweenInputPackets = 0.033f;
}

NetworkManagerClient::NetworkManagerClient() :
mState(NCS_Uninitialized),
mDeliveryNotificationManager(true, false),
m_fLastRoundTripTime(0.f)
{
}

void NetworkManagerClient::StaticInit(const SocketAddress& inServerAddress, const std::string& inName)
{
    sInstance = new NetworkManagerClient();
    return sInstance->Init(inServerAddress, inName);
}

void NetworkManagerClient::Init(const SocketAddress& inServerAddress, const std::string& inName)
{
    NetworkManager::Init(0);
    
    mServerAddress = inServerAddress;
    mState = NCS_SayingHello;
    m_fTimeOfLastHello = 0.f;
    mName = inName;
    
    mAvgRoundTripTime = WeightedTimedMovingAverage(1.f);
}

void NetworkManagerClient::ProcessPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress)
{
    uint32_t packetType;
    inInputStream.Read(packetType);
    
    switch(packetType)
    {
        case kWelcomeCC:
            HandleWelcomePacket(inInputStream);
            break;
        case kStateCC:
            if (mDeliveryNotificationManager.ReadAndProcessState(inInputStream))
            {
                HandleStatePacket(inInputStream);
            }
            break;
    }
}

void NetworkManagerClient::SendOutgoingPackets()
{
    switch (mState)
    {
        case NCS_SayingHello:
            UpdateSayingHello();
            break;
        case NCS_Welcomed:
            UpdateSendingInputPacket();
            break;
        case NCS_Uninitialized:
            break;
    }
}

void NetworkManagerClient::UpdateSayingHello()
{
    float time = Timing::sInstance.GetTime();
    
    if (time > m_fTimeOfLastHello + kTimeBetweenHellos)
    {
        SendHelloPacket();
        m_fTimeOfLastHello = time;
    }
}

void NetworkManagerClient::SendHelloPacket()
{
    OutputMemoryBitStream helloPacket;
    
    helloPacket.Write(kHelloCC);
    helloPacket.Write(mName);
    
    SendPacket(helloPacket, mServerAddress);
}

void NetworkManagerClient::HandleWelcomePacket(InputMemoryBitStream& inInputStream)
{
    if (mState == NCS_SayingHello)
    {
        //if we got a player id, we've been welcomed!
        int playerId;
        inInputStream.Read(playerId);
        m_iPlayerId = playerId;
        mState = NCS_Welcomed;
        LOG("'%s' was welcomed on client as player %d", mName.c_str(), m_iPlayerId);
    }
}

void NetworkManagerClient::HandleStatePacket(InputMemoryBitStream& inInputStream)
{
    if (mState == NCS_Welcomed)
    {
        ReadLastMoveProcessedOnServerTimestamp(inInputStream);
        
        //tell the replication manager to handle the rest...
        mReplicationManagerClient.Read(inInputStream);
    }
}

void NetworkManagerClient::ReadLastMoveProcessedOnServerTimestamp(InputMemoryBitStream& inInputStream)
{
    bool isTimestampDirty;
    inInputStream.Read(isTimestampDirty);
    if (isTimestampDirty)
    {
        inInputStream.Read(m_fLastMoveProcessedByServerTimestamp);
        
        float rtt = Timing::sInstance.GetFrameStartTime() - m_fLastMoveProcessedByServerTimestamp;
        m_fLastRoundTripTime = rtt;
        mAvgRoundTripTime.Update(rtt);
        
        InputManager::sInstance->GetMoveList().RemovedProcessedMoves(m_fLastMoveProcessedByServerTimestamp);
    }
}

void NetworkManagerClient::HandleGameObjectState(InputMemoryBitStream& inInputStream)
{
    //copy the m_networkIdToGameObjectMap so that anything that doesn't get an updated can be destroyed...
    std::unordered_map<int, GameObjectPtr> objectsToDestroy = m_networkIdToGameObjectMap;
    
    int stateCount;
    inInputStream.Read(stateCount);
    if (stateCount > 0)
    {
        for (int stateIndex = 0; stateIndex < stateCount; ++stateIndex)
        {
            int networkId;
            uint32_t fourCC;
            
            inInputStream.Read(networkId);
            inInputStream.Read(fourCC);
            GameObjectPtr go;
            auto itGO = m_networkIdToGameObjectMap.find(networkId);
            //didn't find it, better create it!
            if (itGO == m_networkIdToGameObjectMap.end())
            {
                go = GameObjectRegistry::sInstance->CreateGameObject(fourCC);
                go->SetNetworkId(networkId);
                AddToNetworkIdToGameObjectMap(go);
            }
            else
            {
                //found it
                go = itGO->second;
            }
            
            //now we can update into it
            go->Read(inInputStream);
            objectsToDestroy.erase(networkId);
        }
    }
    
    //anything left gets the axe
    DestroyGameObjectsInMap(objectsToDestroy);
}

void NetworkManagerClient::DestroyGameObjectsInMap(const std::unordered_map<int, GameObjectPtr>& inObjectsToDestroy)
{
    for (auto& pair: inObjectsToDestroy)
    {
        pair.second->SetDoesWantToDie(true);
        //and remove from our map!
        m_networkIdToGameObjectMap.erase(pair.first);
    }
}

void NetworkManagerClient::UpdateSendingInputPacket()
{
    float time = Timing::sInstance.GetTime();
    
    if (time > m_fTimeOfLastInputPacket + kTimeBetweenInputPackets)
    {
        SendInputPacket();
        m_fTimeOfLastInputPacket = time;
    }
}

void NetworkManagerClient::SendInputPacket()
{
    //only send if there's any input to sent!
    const MoveList& moveList = InputManager::sInstance->GetMoveList();
    
    if (moveList.HasMoves())
    {
        OutputMemoryBitStream inputPacket;
        
        inputPacket.Write(kInputCC);
        
        mDeliveryNotificationManager.WriteState(inputPacket);
        
        //eventually write the 3 latest moves so they have three chances to get through...
        int moveCount = moveList.GetMoveCount();
        int firstMoveIndex = moveCount - 3;
        if (firstMoveIndex < 3)
        {
            firstMoveIndex = 0;
        }
        auto move = moveList.begin() + firstMoveIndex;
        
        //only need two bits to write the move count, because it's 0, 1, 2 or 3
        inputPacket.Write(moveCount - firstMoveIndex, 2);
        
        for (; firstMoveIndex < moveCount; ++firstMoveIndex, ++move)
        {
            ///would be nice to optimize the time stamp...
            move->Write(inputPacket);
        }
        
        SendPacket(inputPacket, mServerAddress);
    }
}