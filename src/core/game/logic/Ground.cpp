//
//  Ground.cpp
//  dante
//
//  Created by Stephen Gowen on 7/13/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "Ground.h"

#include "Box2D/Box2D.h"
#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

#include "Robot.h"
#include "Projectile.h"
#include "SpacePirate.h"
#include "Crate.h"

#include <math.h>

Ground::Ground(b2World& world, bool isServer) : Entity(world, GAME_WIDTH / 2.0f, -10.0f, GAME_WIDTH, 23.6f, isServer, constructEntityDef())
{
    // Empty
}

EntityDef Ground::constructEntityDef()
{
    return EntityDef();
}

void Ground::update()
{
    // Empty
}

bool Ground::shouldCollide(Entity *inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    return true;
}

void Ground::handleBeginContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        (static_cast<Robot*>(inEntity))->handleBeginContact(this, inFixtureB, inFixtureA);
    }
    else if (inEntity->getRTTI().derivesFrom(Projectile::rtti))
    {
        (static_cast<Projectile*>(inEntity))->handleBeginContactWithGround(this);
    }
    else if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        (static_cast<SpacePirate*>(inEntity))->handleBeginContact(this, inFixtureB, inFixtureA);
    }
}

void Ground::handleEndContact(Entity* inEntity, b2Fixture* inFixtureA, b2Fixture* inFixtureB)
{
    if (inEntity->getRTTI().derivesFrom(Robot::rtti))
    {
        (static_cast<Robot*>(inEntity))->handleEndContact(this, inFixtureB, inFixtureA);
    }
    else if (inEntity->getRTTI().derivesFrom(SpacePirate::rtti))
    {
        (static_cast<SpacePirate*>(inEntity))->handleEndContact(this, inFixtureB, inFixtureA);
    }
}

uint32_t Ground::getAllStateMask() const
{
    return 0;
}

void Ground::read(InputMemoryBitStream& inInputStream)
{
    // Empty
}

uint32_t Ground::write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState)
{
    uint32_t writtenState = 0;
    
    // Empty
    
    return writtenState;
}

bool Ground::needsMoveReplay()
{
    return false;
}

RTTI_IMPL(Ground, Entity);

NW_TYPE_IMPL(Ground);