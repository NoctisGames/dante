//
//  INetworkManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__INetworkManager__
#define __noctisgames__INetworkManager__

#include "InputMemoryBitStream.h"
#include "SocketAddress.h"
#include "UDPSocket.h"
#include "WeightedTimedMovingAverage.h"

#include <unordered_map>
#include <queue>
#include <list>

class OutputMemoryBitStream;
class Entity;

typedef void (*HandleEntityDeletion)(Entity* inEntity);

class INetworkManager
{
public:
    static const uint32_t kHelloCC = 'HELO';
    static const uint32_t kWelcomeCC = 'WLCM';
    static const uint32_t kStateCC = 'STAT';
    static const uint32_t kInputCC = 'INPT';
    static const int kMaxPacketsPerFrameCount = 10;
    
    virtual void processPacket(InputMemoryBitStream& inInputStream, const SocketAddress& inFromAddress) = 0;
    
    virtual void handleConnectionReset(const SocketAddress& inFromAddress);
    
    bool init(uint16_t inPort, HandleEntityDeletion handleEntityDeletion);
    
    void processIncomingPackets();
    
    void sendPacket(const OutputMemoryBitStream& inOutputStream, const SocketAddress& inFromAddress);
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
    Entity* getEntity(int inNetworkId) const;
    
    void addToNetworkIdToEntityMap(Entity* inEntity);
    
    void removeFromNetworkIdToEntityMap(Entity* inEntity);
    
protected:
    // ctor, copy ctor, and assignment should be private in a Singleton
    INetworkManager();
    virtual ~INetworkManager();
    INetworkManager(const INetworkManager&);
    INetworkManager& operator=(const INetworkManager&);
    
private:
    class ReceivedPacket;
    
    HandleEntityDeletion m_handleEntityDeletion;
    
    std::queue<ReceivedPacket, std::list<ReceivedPacket>> m_packetQueue;
    
    UDPSocket* m_socket;
    
    WeightedTimedMovingAverage m_bytesReceivedPerSecond;
    WeightedTimedMovingAverage m_bytesSentPerSecond;
    
    int m_bytesSentThisFrame;
    
    void updateBytesSentLastFrame();
    
    void readIncomingPacketsIntoQueue();
    
    void processQueuedPackets();
    
    class ReceivedPacket
    {
    public:
        ReceivedPacket(float inReceivedTime, InputMemoryBitStream& inInputMemoryBitStream, const SocketAddress& inAddress);
        
        const SocketAddress& getFromAddress() const;
        float getReceivedTime()	const;
        InputMemoryBitStream& getPacketBuffer();
        
    private:
        float m_fReceivedTime;
        InputMemoryBitStream m_packetBuffer;
        SocketAddress m_fromAddress;
    };
};

#endif /* defined(__noctisgames__INetworkManager__) */