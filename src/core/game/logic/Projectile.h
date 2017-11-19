//
//  Projectile.h
//  dante
//
//  Created by Stephen Gowen on 6/4/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Projectile__
#define __noctisgames__Projectile__

#include "framework/entity/Entity.h"

#include "game/logic/GameConstants.h"

#include "Box2D/Common/b2Math.h"

class Robot;
class SpacePirate;
class SpacePirateChunk;
class Ground;
class Crate;

class Projectile : public Entity
{
    RTTI_DECL;
    
    NW_TYPE_DECL(NW_TYPE_Projectile);
    
public:
    enum ProjectileReplicationState
    {
        PRJC_PlayerInfo = 1 << 0,
        PRJC_Pose = 1 << 1,
        
        PRJC_AllState = PRJC_PlayerInfo | PRJC_Pose
    };
    
    enum ProjectileState
    {
        ProjectileState_Waiting = 1,
        ProjectileState_Active = 2,
        ProjectileState_Exploding = 3
    };
    
    Projectile(b2World& world, bool isServer);
    
    virtual EntityDef constructEntityDef();
    
    virtual void update();
    
    virtual bool shouldCollide(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual void handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB);
    
    virtual uint32_t getAllStateMask() const;
    
    virtual void read(InputMemoryBitStream& inInputStream);
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState);
    
    virtual bool needsMoveReplay();
    
    void initFromShooter(Robot* inRobot);
    
    void fire(Robot* inRobot);
    
    ProjectileState getState();
    
    void setPlayerId(uint32_t inPlayerId);
    
    uint32_t getPlayerId() const;
    
    bool isFacingLeft();
    
private:
    uint32_t _playerId;
    bool _isFacingLeft;
    bool _hasMadeContact;
    
    ProjectileState _state;
    
    // Cached Last Known Values (from previous frame)
    ProjectileState _stateLastKnown;
    
    void handleBeginContactWithSpacePirate(SpacePirate* inEntity);
    
    void explode();
};

#endif /* defined(__noctisgames__Projectile__) */
