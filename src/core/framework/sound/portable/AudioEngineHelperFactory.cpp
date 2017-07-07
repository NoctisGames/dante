//
//  AudioEngineHelperFactory.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "AudioEngineHelperFactory.h"

#if defined __APPLE__
    #include "TargetConditionals.h"
    #if TARGET_OS_IPHONE
        #import "AppleAudioEngineHelper.h"
    #elif TARGET_OS_OSX
        #import "MacAudioEngineHelper.h"
    #endif
#elif defined __ANDROID__
    #include "AndroidAudioEngineHelper.h"
#elif defined _WIN32
    #include "WinAudioEngineHelper.h"
#endif

#include <assert.h>

AudioEngineHelperFactory* AudioEngineHelperFactory::getInstance()
{
    static AudioEngineHelperFactory instance = AudioEngineHelperFactory();
    return &instance;
}

IAudioEngineHelper* AudioEngineHelperFactory::createAudioEngineHelper()
{
#if defined __APPLE__
    #if TARGET_OS_IPHONE
        return AppleAudioEngineHelper::getInstance();
    #elif TARGET_OS_OSX
        return MacAudioEngineHelper::getInstance();
    #endif
#elif defined __ANDROID__
    return AndroidAudioEngineHelper::getInstance();
#elif defined _WIN32
	return WinAudioEngineHelper::getInstance();
#endif
    
    assert(false);
}

AudioEngineHelperFactory::AudioEngineHelperFactory()
{
    // Hide Constructor for Singleton
}
