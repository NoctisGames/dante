//
//  MainRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainRenderer__
#define __dante__MainRenderer__

#include "Renderer.h"

#include "FontAlign.h"

#include <string>

class TextureWrapper;
class Font;
struct b2Vec2;
class World;

class MainRenderer : public Renderer
{
public:
    MainRenderer(int maxBatchSize);
    
    virtual ~MainRenderer();
    
    virtual void createDeviceDependentResources();
    
    virtual void releaseDeviceDependentResources();
    
    virtual void render(int flags = 0);
    
private:
    TextureWrapper* _characters;
    TextureWrapper* _misc;
    TextureWrapper* _bg1;
    TextureWrapper* _bg2;
    TextureWrapper* _cover;
    Font* _font;
    NGRect* _camBounds;
    
    void renderBackground();
    
    void renderWorld();
    
    void renderEntities(World* world, bool isServer);
    
    void renderAtmosphere();
    
    void renderUI(int engineState);
    
    void renderMainMenuSteamOffText();
    
    void renderMainMenuSteamOnText();
    
    void renderStartingServerText();
    
    void renderEnterUsernameText();
    
    void renderJoiningLocalServerByIPText();
    
    void renderJoiningServerText();
    
    void renderServerJoinedText();
    
    void renderText(const std::string& inStr, const b2Vec2& origin, const Color& inColor, int justification = FONT_ALIGN_LEFT);
    
    void updateCamera();
};

#endif /* defined(__dante__MainRenderer__) */
