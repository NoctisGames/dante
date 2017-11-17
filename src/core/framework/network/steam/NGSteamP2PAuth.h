//
//  NGSteamP2PAuth.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 6/20/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGSteamP2PAuth__
#define __noctisgames__NGSteamP2PAuth__

#include "NGSteam.h"
#include "FrameworkConstants.h"
#include "MathUtil.h"

class InputMemoryBitStream;
class INetworkHelper;
class NGSteamAddress;

class NGSteamP2PAuthPlayer;
class NGSteamP2PNetworkTransport;
class MsgP2PSendingTicket;

class NGSteamP2PAuth
{
public:
    NGSteamP2PAuthPlayer *_rgpP2PAuthPlayer[MAX_NUM_PLAYERS_PER_SERVER];
    MsgP2PSendingTicket *_rgpQueuedMessage[MAX_NUM_PLAYERS_PER_SERVER];
    NGSteamP2PNetworkTransport *_networkTransport;
    
    NGSteamP2PAuth(INetworkHelper* networkHelper);
    
    ~NGSteamP2PAuth();
    
    void playerDisconnect(uint8_t iSlot);
    void endGame();
    void startAuthPlayer(uint8_t iSlot, CSteamID steamID);
    void registerPlayer(uint8_t iSlot, CSteamID steamID);
    bool handleMessage(uint32_t packetType, InputMemoryBitStream& inInputStream);
    void internalinitPlayer(uint8_t iSlot, CSteamID steamID, bool bStartAuthProcess);
};

class NGSteamP2PAuthPlayer
{
public:
    CSteamID _steamID;
    
	NGSteamP2PAuthPlayer(NGSteamP2PNetworkTransport *pNetworkTransport);
    
	~NGSteamP2PAuthPlayer();
	
    void endGame();
	void initPlayer(CSteamID steamID);
	void startAuthPlayer();
	bool isAuthOk();
	bool handleMessage(MsgP2PSendingTicket* msg);

	STEAM_CALLBACK(NGSteamP2PAuthPlayer, OnBeginAuthResponse, ValidateAuthTicketResponse_t, _CallbackBeginAuthResponse);
    
private:
	bool _bSentTicket;
	bool _bSubmittedHisTicket;
	bool _bHaveAnswer;
	float _connectTime;
	float _ticketTime;
	float _answerTime;
	uint32 _cubTicketIGaveThisUser;
	uint8 _rgubTicketIGaveThisUser[1024];
	uint32 _cubTicketHeGaveMe;
	uint8 _rgubTicketHeGaveMe[1024];
	HAuthTicket _hAuthTicketIGaveThisUser;
	EBeginAuthSessionResult _eBeginAuthSessionResult;
	EAuthSessionResponse _eAuthSessionResponse;

	NGSteamP2PNetworkTransport *_networkTransport;
};

class NGSteamP2PNetworkTransport
{
public:
	NGSteamP2PNetworkTransport(INetworkHelper* networkHelper);
    
    ~NGSteamP2PNetworkTransport();
    
	void sendTicket(CSteamID steamIDFrom, CSteamID steamIDTo, uint32 cubTicket, uint8 *pubTicket);
	void closeConnection(CSteamID steamID);

	STEAM_CALLBACK(NGSteamP2PNetworkTransport, onP2PSessionRequest, P2PSessionRequest_t, _CallbackP2PSessionRequest);
	STEAM_CALLBACK(NGSteamP2PNetworkTransport, onP2PSessionConnectFail, P2PSessionConnectFail_t, _CallbackP2PSessionConnectFail);
    
private:
    INetworkHelper* _networkHelper;
    NGSteamAddress* _outgoingPacketAddress;
};

class MsgP2PSendingTicket
{
public:
    MsgP2PSendingTicket();
    
    DWORD getMessageType();
    
    void setToken(const char *pchToken, uint32 unLen);
    uint32 getTokenLen();
    const char* getTokenPtr();
    
    void setSteamID(uint64 ulSteamID);
    uint64 getSteamID();
    
private:
    const DWORD _dwMessageType;
    uint32 _uTokenLen;
    char _rgchToken[1024];
    uint64 _ulSteamID;
};

#endif /* defined(__noctisgames__NGSteamP2PAuth__) */
