//
//  SocketServerHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SocketServerHelper__
#define __noctisgames__SocketServerHelper__

#include "ServerHelper.h"

#include "PacketHandler.h"

class SocketServerHelper : public ServerHelper
{
public:
    SocketServerHelper(uint16_t inPort, ProcessPacketFunc inProcessPacketFunc, HandleNoResponseFunc inHandleNoResponseFunc, HandleConnectionResetFunc inHandleConnectionResetFunc, GetClientProxyFunc inGetClientProxyFunc, HandleClientDisconnectedFunc inHandleClientDisconnectedFunc);
    
    virtual ~SocketServerHelper();
    
    virtual void processSpecialPacket(uint32_t packetType, InputMemoryBitStream& inInputStream, MachineAddress* inFromAddress);
    
    virtual void onClientDisconnected(ClientProxy* clientProxy);
    
    virtual MachineAddress* getServerAddress();
    
    virtual bool isConnected();
};

#endif /* defined(__noctisgames__SocketServerHelper__) */
