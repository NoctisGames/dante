//
//  MainAssets.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/graphics/portable/MainAssets.h"

#include "framework/graphics/portable/TextureRegion.h"

#include "framework/graphics/portable/Assets.h"
#include "framework/util/FlagUtil.h"

#define FLAG_TEXTURE_SET_COMPRESSED 1
#define FLAG_TEXTURE_SET_DESKTOP 2
#define FLAG_TEXTURE_SET_GAME_PAD 4

MainAssets* MainAssets::getInstance()
{
    static MainAssets instance = MainAssets();
    return &instance;
}

bool MainAssets::isUsingCompressedTextureSet()
{
    return FlagUtil::isFlagSet(_textureSetFlag, FLAG_TEXTURE_SET_COMPRESSED);
}

void MainAssets::setUsingCompressedTextureSet(bool isUsingCompressedTextureSet)
{
    if (isUsingCompressedTextureSet)
    {
        FlagUtil::setFlag(_textureSetFlag, FLAG_TEXTURE_SET_COMPRESSED);
    }
    else
    {
        FlagUtil::removeFlag(_textureSetFlag, FLAG_TEXTURE_SET_COMPRESSED);
    }
}

bool MainAssets::isUsingDesktopTextureSet()
{
    return FlagUtil::isFlagSet(_textureSetFlag, FLAG_TEXTURE_SET_DESKTOP);
}

void MainAssets::setUsingDesktopTextureSet(bool isUsingDesktopTextureSet)
{
    if (isUsingDesktopTextureSet)
    {
        FlagUtil::setFlag(_textureSetFlag, FLAG_TEXTURE_SET_DESKTOP);
    }
    else
    {
        FlagUtil::removeFlag(_textureSetFlag, FLAG_TEXTURE_SET_DESKTOP);
    }
}

bool MainAssets::isUsingGamePadTextureSet()
{
    return FlagUtil::isFlagSet(_textureSetFlag, FLAG_TEXTURE_SET_GAME_PAD);
}

void MainAssets::setUsingGamePadTextureSet(bool isUsingGamePadTextureSet)
{
    if (isUsingGamePadTextureSet)
    {
        FlagUtil::setFlag(_textureSetFlag, FLAG_TEXTURE_SET_GAME_PAD);
    }
    else
    {
        FlagUtil::removeFlag(_textureSetFlag, FLAG_TEXTURE_SET_GAME_PAD);
    }
}

MainAssets::MainAssets() : _textureSetFlag(0)
{
    // Empty
}
