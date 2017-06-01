//
//  RoboCatClient.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCatClient__
#define __noctisgames__RoboCatClient__

#include "RoboCat.h"

class RoboCatClient : public RoboCat
{
    RTTI_DECL;
    
public:
    static GameObject* create();
    
    virtual void update() override;
    virtual void handleDying() override;
    
    virtual void read(InputMemoryBitStream& inInputStream) override;
    
    void doClientSidePredictionAfterReplicationForLocalCat(uint32_t inReadState);
    void doClientSidePredictionAfterReplicationForRemoteCat(uint32_t inReadState);
    
protected:
    RoboCatClient();
    
private:
    void interpolateClientSidePrediction(float inOldRotation, Vector2& inOldLocation, Vector2& inOldVelocity, bool inIsForRemoteCat);
    
    float m_fTimeLocationBecameOutOfSync;
    float m_fTimeVelocityBecameOutOfSync;
};

#endif /* defined(__noctisgames__RoboCatClient__) */
