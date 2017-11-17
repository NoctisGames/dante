//
//  MainEngine.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainEngine__
#define __dante__MainEngine__

#include "Engine.h"

#ifdef NG_STEAM
#include "NGSteam.h"
#endif

#include "RTTI.h"

class Server;
class JsonFile;
class Entity;

class MainEngine : public Engine
{
    RTTI_DECL;
    
public:
    MainEngine();
    
    virtual ~MainEngine();
    
	virtual void update(float deltaTime);
    
protected:
    virtual void onFrame();
    
private:
    JsonFile* m_config;
    
    std::string m_serverIPAddress;
    std::string m_name;
#ifdef NG_STEAM
    CSteamID m_serverSteamID;
#endif
    bool m_isSteam;
    
    void handleNonMoveInput();
    
    void activateSteam();
    
    void handleSteamGameServices();
    
    void deactivateSteam();
    
    void startServer();
    
    void joinServer();
    
    void disconnect();
};

#endif /* defined(__dante__MainEngine__) */
