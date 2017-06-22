//
//  SpacePirate.h
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__SpacePirate__
#define __noctisgames__SpacePirate__

#include "Entity.h"

#include "GameConstants.h"

#include "RTTI.h"

#include <stdint.h>

class Robot;

class SpacePirate : public Entity
{
    RTTI_DECL;
    
    NETWORK_TYPE_DECL(NETWORK_TYPE_SpacePirate);
    
public:
    enum SpacePirateReplicationState
    {
        SPCP_Pose = 1 << 0,
        SPCP_Color = 1 << 1,
        SPCP_Health = 1 << 2,
        SPCP_Size = 1 << 2,
        
        SPCP_AllState = SPCP_Pose | SPCP_Color | SPCP_Health | SPCP_Size
    };
    
    static Entity* staticCreateClient();
    
    static Entity* staticCreateServer();
    
    virtual void onDeletion();
    
    virtual void update();
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    void init(float x, float y, float speed, int scale, uint8_t health);
    
    void takeDamage(bool isHeadshot);
    
    uint8_t getHealth();
    
    float getSpeed();
    
    bool isFacingLeft();
    
private:
    bool m_isServer;
    float m_fSpeed;
    uint8_t m_iHealth;
    bool m_isFacingLeft;
    bool m_isGrounded;
    bool m_isFalling;
    bool m_isJumping;
    
    float m_fTimeForNextJump;
    float m_fJumpSpeed;
    float m_fStartingHealth;
    
    //bounce fraction when hitting various things
    float m_fRobotRestitution;
    
    void processAI();
    
    void updateInternal(float inDeltaTime);
    
    void processCollisions();
    
    void processCollisionsWithScreenWalls();
    
    SpacePirate(bool isServer);
};

#endif /* defined(__noctisgames__SpacePirate__) */