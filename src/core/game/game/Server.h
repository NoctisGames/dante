//
//  Server.h
//  dante
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Server__
#define __noctisgames__Server__

#include <framework/util/NGPool.h>
#include <game/game/GameInputState.h>

#include <string>
#include <vector>

enum ServerFlags
{
    ServerFlag_Steam =       1 << 0,
    ServerFlag_TestSession = 1 << 1
};

class World;
class ClientProxy;
class Entity;
class Timing;
class EntityIDManager;

class Server
{
public:
    static void create(uint32_t flags, void* data = NULL);
    static Server* getInstance();
    static void destroy();
    
    static void sHandleDynamicEntityCreatedOnServer(Entity* entity);
    static void sHandleDynamicEntityDeletedOnServer(Entity* entity);
    
    static void sHandleNewClient(uint8_t playerId, std::string playerName);
    static void sHandleLostClient(ClientProxy* inClientProxy, uint8_t index);
    static InputState* sHandleInputStateCreation();
    static void sHandleInputStateRelease(InputState* inputState);
    
    void update();
    uint8_t getPlayerIdForRobotBeingCreated();
    void toggleMap();
    uint32_t getFlags();
    World* getWorld();

private:
    static Server* s_instance;
    
    uint32_t _flags;
    void* _data;
    Timing* _timing;
    EntityIDManager* _entityIDManager;
    World* _world;
    NoctisGames::NGPool<GameInputState> _inputStates;
    uint32_t _map;
    
    void handleNewClient(uint8_t playerId, std::string playerName);
    void handleLostClient(ClientProxy* inClientProxy, uint8_t index);
    InputState* handleInputStateCreation();
    void handleInputStateRelease(InputState* inputState);
    void deleteRobotWithPlayerId(uint8_t playerId);
    void spawnRobotForPlayer(uint8_t inPlayerId, std::string inPlayerName);
    void loadMap();
    void handleDirtyStates(std::vector<Entity*>& entities);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    Server(uint32_t flags, void* data = NULL);
    ~Server();
    Server(const Server&);
    Server& operator=(const Server&);
};

#endif /* defined(__noctisgames__Server__) */
