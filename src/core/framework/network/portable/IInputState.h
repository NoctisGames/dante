//
//  IInputState.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__IInputState__
#define __noctisgames__IInputState__

#include "RTTI.h"

class OutputMemoryBitStream;
class InputMemoryBitStream;

class IInputState
{
    RTTI_DECL;
    
public:
    IInputState();
    
    virtual ~IInputState();
    
    virtual bool write(OutputMemoryBitStream& inOutputStream) const = 0;
    virtual bool read(InputMemoryBitStream& inInputStream) = 0;
    
protected:
    void writeSignedBinaryValue(OutputMemoryBitStream& inOutputStream, float inValue) const;
    
    void readSignedBinaryValue(InputMemoryBitStream& inInputStream, float& outValue) const;
};

#endif /* defined(__noctisgames__IInputState__) */