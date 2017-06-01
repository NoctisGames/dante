//
//  InputManager.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputManager__
#define __noctisgames__InputManager__

#include "InputAction.h"
#include "InputState.h"
#include "MoveList.h"

class Move;

class InputManager
{
public:
    static InputManager* getInstance();
    
    void handleInput(EInputAction inInputAction, char inKeyCode);
    
    void update();
    
    MoveList& getMoveList();
    
    const Move* getAndClearPendingMove();
    
private:
    InputState m_currentState;
    MoveList m_moveList;
    float m_fNextTimeToSampleInput;
    const Move* m_pendingMove;
    
    bool isTimeToSampleInput();
    const Move& sampleInputAsMove();
    
    // ctor, copy ctor, and assignment should be private in a Singleton
    InputManager();
    InputManager(const InputManager&);
    InputManager& operator=(const InputManager&);
};

#endif /* defined(__noctisgames__InputManager__) */
