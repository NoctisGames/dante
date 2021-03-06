//
//  MainEngineController.cpp
//  dante
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include <game/config/MainEngineController.h>

#include <framework/util/NGExtension.h>
#include <framework/studio/StudioEngine.h>
#include <framework/entity/EntityMapper.h>
#include <framework/util/InstanceManager.h>
#include <framework/util/Timing.h>
#include <framework/entity/EntityIDManager.h>
#include <framework/util/Constants.h>
#include <game/title/TitleEngine.h>
#include <game/game/GameEngine.h>
#include <game/entity/BasicFollowAndAttackController.h>
#include <game/entity/DestructibleController.h>
#include <game/entity/PlayerController.h>

IMPL_RTTI(MainEngineController, EngineController);

MainEngineController::MainEngineController()
{
    NoctisGames::NGExtension::setInstance(NoctisGames::DefaultNGExtension::getInstance());
    
    static Timing timingServer;
    static Timing timingClient;
    static EntityIDManager entityIDManagerServer;
    static EntityIDManager entityIDManagerClient;
    static EntityIDManager entityIDManagerStudio;
    
    INSTANCE_MANAGER->registerInstance(INSTANCE_TIME_SERVER, &timingServer);
    INSTANCE_MANAGER->registerInstance(INSTANCE_TIME_CLIENT, &timingClient);
    INSTANCE_MANAGER->registerInstance(INSTANCE_ENTITY_ID_MANAGER_SERVER, &entityIDManagerServer);
    INSTANCE_MANAGER->registerInstance(INSTANCE_ENTITY_ID_MANAGER_CLIENT, &entityIDManagerClient);
    INSTANCE_MANAGER->registerInstance(INSTANCE_ENTITY_ID_MANAGER_STUDIO, &entityIDManagerStudio);
    
    EntityMapper::getInstance()->registerFunction("BasicFollowAndAttack", BasicFollowAndAttackController::create);
    EntityMapper::getInstance()->registerFunction("BasicFollowAndAttack", BasicFollowAndAttackNetworkController::create);
    
    EntityMapper::getInstance()->registerFunction("Destructible", DestructibleController::create);
    EntityMapper::getInstance()->registerFunction("Destructible", DestructibleNetworkController::create);
    
    EntityMapper::getInstance()->registerFunction("Player", PlayerController::create);
    EntityMapper::getInstance()->registerFunction("Player", PlayerNetworkController::create);
    
    TitleEngine::create();
    GameEngine::create();
    StudioEngine::create(GameEngine::sHandleTest);
}

MainEngineController::~MainEngineController()
{
    StudioEngine::destroy();
    GameEngine::destroy();
    TitleEngine::destroy();
}

EngineState* MainEngineController::getInitialState()
{
    return TitleEngine::getInstance();
}
