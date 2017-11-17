//
//  TextureRegion.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "TextureRegion.h"

TextureRegion::TextureRegion(std::string textureName, int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight) : _textureName(textureName)
{
    init(x, y, regionWidth, regionHeight, textureWidth, textureHeight);
}

void TextureRegion::init(int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight)
{
    _x = (float) x;
    _y = (float) y;
    _regionWidth = (float) regionWidth;
    _regionHeight = (float) regionHeight;
    _textureWidth = (float) textureWidth;
    _textureHeight = (float) textureHeight;
    
    u1 = _x / _textureWidth;
    v1 = _y / _textureHeight;
    u2 = u1 + _regionWidth / _textureWidth;
    v2 = v1 + _regionHeight / _textureHeight;
}

void TextureRegion::initX(int x)
{
    _x = (float) x;
    
    u1 = _x / _textureWidth;
    u2 = u1 + _regionWidth / _textureWidth;
}

void TextureRegion::initY(int y)
{
    _y = (float) y;
    
    v1 = _y / _textureHeight;
    v2 = v1 + _regionHeight / _textureHeight;
}

std::string& TextureRegion::getTextureName()
{
    return _textureName;
}
