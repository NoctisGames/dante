//
//  InputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__InputState__
#define __noctisgames__InputState__

class OutputMemoryBitStream;
class InputMemoryBitStream;

#include "IInputState.h"

#include "RTTI.h"

class InputState : public IInputState
{
    RTTI_DECL;
    
public:
    InputState();
    
    virtual ~InputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const;
    virtual bool read(InputMemoryBitStream& inInputStream);
    
    void copyTo(InputState* inInputState);
    
    bool isEqualTo(InputState* inInputState);
    
    float getDesiredHorizontalDelta() const;
    
    float getDesiredJumpIntensity() const;
    
    bool isShooting() const;
    
private:
    friend class InputManager;
    
    float m_fDesiredRightAmount;
    float m_fDesiredLeftAmount;
    float m_fDesiredJumpIntensity;
    bool m_isShooting;
};

#endif /* defined(__noctisgames__InputState__) */