//
//  RoboCatServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCatServer__
#define __noctisgames__RoboCatServer__

#include "RoboCat.h"

#include "NetworkManagerServer.h"

class RoboCatServer : public RoboCat
{
public:
    static GameObjectPtr StaticCreate() { return NetworkManagerServer::sInstance->RegisterAndReturn(new RoboCatServer()); }
    virtual void HandleDying() override;
    
    virtual void Update() override;
    
    void TakeDamage(int inDamagingPlayerId);
    
protected:
    RoboCatServer();
    
private:
    float mTimeOfNextShot;
    float mTimeBetweenShots;
};

#endif /* defined(__noctisgames__RoboCatServer__) */
