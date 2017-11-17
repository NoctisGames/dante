//
//  TextureRegion.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__TextureRegion__
#define __noctisgames__TextureRegion__

#include <string>

class TextureRegion
{
public:
    float u1, v1, u2, v2;
    float _x, _y, _regionWidth, _regionHeight, _textureWidth, _textureHeight;
    
    TextureRegion(std::string textureName, int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight);
    
    void init(int x, int y, int regionWidth, int regionHeight, int textureWidth, int textureHeight);
    
    // Useful for parallax
    void initX(int x);
    void initY(int y);
    
    std::string& getTextureName();
    
private:
    std::string _textureName;
};

#endif /* defined(__noctisgames__TextureRegion__) */
