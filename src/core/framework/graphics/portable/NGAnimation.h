//
//  NGAnimation.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 9/3/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGAnimation__
#define __noctisgames__NGAnimation__

#include <vector>
#include <string>

class TextureRegion;

class NGAnimation
{
public:
    NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool looping, int numFrames);
    
    NGAnimation(std::string textureName, int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, bool looping, float frameTime, int numFrames, int firstLoopingFrame = 0, int xPadding = 0, int yPadding = 0);
    
    ~NGAnimation();
    
    void setFrameTimes(int numFrames, ...);
    
    TextureRegion& getTextureRegion(float stateTime);
    
    TextureRegion& getTextureRegion(int keyFrameNumber);
    
    int getKeyFrameNumber(float stateTime);
    
    bool hasFrameTimes();
    
    std::string& getTextureName() { return _textureName; }
    
private:
    std::vector<TextureRegion> _textureRegions;
    std::vector<float> _frameTimes;
    std::string _textureName;
    float _cycleTime;
    int _firstLoopingFrame;
    bool _isLooping;
    
    void loadTextureRegions(int x, int y, int regionWidth, int regionHeight, int animationWidth, int animationHeight, int textureWidth, int textureHeight, int numFrames, int xPadding = 0, int yPadding = 0);
};

#endif /* defined(__noctisgames__NGAnimation__) */