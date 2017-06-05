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

InputState::InputState() : IInputState(),
m_fDesiredRightAmount(0),
m_fDesiredLeftAmount(0),
m_fDesiredJumpIntensity(0),
m_isShooting(false)
{
    // Empty
}

InputState::~InputState()
{
    // Empty
}

bool InputState::write(OutputMemoryBitStream& inOutputStream) const
{
    inOutputStream.writeSignedBinaryValue(getDesiredHorizontalDelta());
    inOutputStream.writeSignedBinaryValue(m_fDesiredJumpIntensity);
    inOutputStream.write(m_isShooting);
    
    return false;
}

bool InputState::read(InputMemoryBitStream& inInputStream)
{
    inInputStream.readSignedBinaryValue(m_fDesiredRightAmount);
    inInputStream.readSignedBinaryValue(m_fDesiredJumpIntensity);
    inInputStream.read(m_isShooting);
    
    return true;
}

bool InputState::isEqualTo(InputState* inInputState)
{
    return inInputState->m_fDesiredRightAmount == m_fDesiredRightAmount
    && inInputState->m_fDesiredLeftAmount == m_fDesiredLeftAmount
    && inInputState->m_fDesiredJumpIntensity == m_fDesiredJumpIntensity
    && inInputState->m_isShooting == m_isShooting;
}

void InputState::copyTo(InputState* inInputState)
{
    inInputState->m_fDesiredRightAmount = m_fDesiredRightAmount;
    inInputState->m_fDesiredLeftAmount = m_fDesiredLeftAmount;
    inInputState->m_fDesiredJumpIntensity = m_fDesiredJumpIntensity;
    inInputState->m_isShooting = m_isShooting;
}

float InputState::getDesiredHorizontalDelta() const
{
    return m_fDesiredRightAmount - m_fDesiredLeftAmount;
}

float InputState::getDesiredJumpIntensity() const
{
    return m_fDesiredJumpIntensity;
}

bool InputState::isShooting() const
{
    return m_isShooting;
}

RTTI_IMPL(InputState, IInputState);