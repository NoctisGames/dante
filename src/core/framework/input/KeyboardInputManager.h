//
//  KeyboardInputManager.h
//  noctisgames
//
//  Created by Stephen Gowen on 2/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__KeyboardInputManager__
#define __noctisgames__KeyboardInputManager__

#include <framework/util/NGRollingPool.h>

#include <vector>

#define KEYBOARD_INPUT_MANAGER (KeyboardInputManager::getInstance())

class KeyboardEvent;

class KeyboardInputManager
{
public:
    static KeyboardInputManager* getInstance();
    
    void onInput(unsigned short key, bool isUp = false);
    
    void process();
    
    std::vector<KeyboardEvent*>& getEvents();
    
private:
    NGRollingPool<KeyboardEvent>* _pool;
    std::map<unsigned short, bool> _lastKnownKeyStates;
    
    void addEvent(unsigned short key, bool isUp);
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    KeyboardInputManager();
    ~KeyboardInputManager();
    KeyboardInputManager(const KeyboardInputManager&);
    KeyboardInputManager& operator=(const KeyboardInputManager&);
};

#endif /* defined(__noctisgames__KeyboardInputManager__) */
