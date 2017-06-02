//
//  UDPSocket.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "UDPSocket.h"

#include "SocketAddress.h"

#include "SocketUtil.h"
#include "StringUtil.h"

int UDPSocket::Bind(const SocketAddress& inBindAddress)
{
    int error = bind(m_socket, &inBindAddress.mSockAddr, inBindAddress.GetSize());
    if (error != 0)
    {
        SocketUtil::ReportError("UDPSocket::Bind");
        
        return SocketUtil::GetLastError();
    }
    
    return NO_ERROR;
}

int UDPSocket::SendTo(const void* inToSend, int inLength, const SocketAddress& inToAddress)
{
    long byteSentCount = sendto(m_socket,
                                static_cast<const char*>(inToSend),
                                inLength,
                                0, &inToAddress.mSockAddr, inToAddress.GetSize());
    
    if (byteSentCount <= 0)
    {
        //we'll return error as negative number to indicate less than requested amount of bytes sent...
        SocketUtil::ReportError("UDPSocket::SendTo");
        
        return -SocketUtil::GetLastError();
    }
    else
    {
        return static_cast<int>(byteSentCount);
    }
}

int UDPSocket::ReceiveFrom(void* inToReceive, int inMaxLength, SocketAddress& outFromAddress)
{
    socklen_t fromLength = outFromAddress.GetSize();
    
    long readByteCount = recvfrom(m_socket,
                                  static_cast<char*>(inToReceive),
                                  inMaxLength,
                                  0, &outFromAddress.mSockAddr, &fromLength);
    
    if (readByteCount >= 0)
    {
        return static_cast<int>(readByteCount);
    }
    else
    {
        int error = SocketUtil::GetLastError();
        
        if (error == WSAEWOULDBLOCK)
        {
            return 0;
        }
        else if (error == WSAECONNRESET)
        {
            //this can happen if a client closed and we haven't DC'd yet.
            //this is the ICMP message being sent back saying the port on that computer is closed
            LOG("Connection reset from %s", outFromAddress.ToString().c_str());
            
            return -WSAECONNRESET;
        }
        else
        {
            SocketUtil::ReportError("UDPSocket::ReceiveFrom");
            
            return -error;
        }
    }
}

UDPSocket::~UDPSocket()
{
#if _WIN32
    closesocket(m_socket);
#else
    close(m_socket);
#endif
}

int UDPSocket::SetNonBlockingMode(bool inShouldBeNonBlocking)
{
#if _WIN32
    u_long arg = inShouldBeNonBlocking ? 1 : 0;
    int result = ioctlsocket(m_socket, FIONBIO, &arg);
#else
    int flags = fcntl(m_socket, F_GETFL, 0);
    flags = inShouldBeNonBlocking ? (flags | O_NONBLOCK) : (flags & ~O_NONBLOCK);
    int result = fcntl(m_socket, F_SETFL, flags);
#endif
    
    if (result == SOCKET_ERROR)
    {
        SocketUtil::ReportError("UDPSocket::SetNonBlockingMode");
        
        return SocketUtil::GetLastError();
    }
    else
    {
        return NO_ERROR;
    }
}

UDPSocket::UDPSocket(SOCKET inSocket) : m_socket(inSocket)
{
    // Empty
}
