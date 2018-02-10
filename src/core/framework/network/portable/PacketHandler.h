//
//  PacketHandler.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__PacketHandler__
#define __noctisgames__PacketHandler__

#include <framework/network/portable/InputMemoryBitStream.h>

#include <queue>
#include <list>

class OutputMemoryBitStream;
class WeightedTimedMovingAverage;
class MachineAddress;

typedef void (*ProcessPacketFunc)(InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
typedef void (*HandleNoResponseFunc)();
typedef void (*HandleConnectionResetFunc)(MachineAddress* inFromAddress);

class PacketHandler
{
public:
    PacketHandler(bool isServer, ProcessPacketFunc processPacketFunc, HandleNoResponseFunc handleNoResponseFunc, HandleConnectionResetFunc handleConnectionResetFunc);
    
    virtual ~PacketHandler();
    
    virtual void sendPacket(const OutputMemoryBitStream& inOutputStream, MachineAddress* inFromAddress) = 0;
    
    void processIncomingPackets();
    
    const WeightedTimedMovingAverage& getBytesReceivedPerSecond() const;
    
    const WeightedTimedMovingAverage& getBytesSentPerSecond() const;
    
protected:
    ProcessPacketFunc _processPacketFunc;
    HandleNoResponseFunc _handleNoResponseFunc;
    HandleConnectionResetFunc _handleConnectionResetFunc;
    int _bytesSentThisFrame;
    bool _isServer;
    
    virtual void readIncomingPacketsIntoQueue() = 0;
    
    virtual void processQueuedPackets() = 0;
    
    void updateBytesSentLastFrame();
    
    void updateBytesReceivedLastFrame(int totalReadByteCount);
    
private:
    WeightedTimedMovingAverage* _bytesReceivedPerSecond;
    WeightedTimedMovingAverage* _bytesSentPerSecond;
};

#endif /* defined(__noctisgames__PacketHandler__) */
