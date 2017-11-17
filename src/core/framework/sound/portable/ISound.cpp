//
//  ISound.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/10/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "ISound.h"

ISound::ISound(int soundId) : _soundId(soundId)
{
    // Empty
}

ISound::~ISound()
{
    // Empty
}

int ISound::getSoundId()
{
    return _soundId;
}
