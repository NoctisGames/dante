//
//  InputState.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "InputState.h"

#include "OutputMemoryBitStream.h"
#include "InputMemoryBitStream.h"

InputState::InputState() : _isInUse(false)
{
    // Empty
}

InputState::~InputState()
{
    // Empty
}

void InputState::setInUse(bool inUse)
{
    _isInUse = inUse;
}

bool InputState::isInUse() const
{
    return _isInUse;
}

RTTI_IMPL_NOPARENT(InputState);