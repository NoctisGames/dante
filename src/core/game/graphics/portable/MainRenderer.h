//
//  MainRenderer.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__MainRenderer__
#define __noctisgames__MainRenderer__

#include "framework/graphics/portable/Renderer.h"

#include "framework/graphics/portable/FontAlign.h"

#include <string>

class TextureManager;
class SpriteBatcher;
class QuadBatcher;
class LineBatcher;
class CircleBatcher;
class NGShaderLoader;
class RendererHelper;
class NGShader;
class Entity;
class TextureRegion;
class Color;
class NGRect;
class NGTexture;
class Box2DDebugRenderer;
class Font;
struct b2Vec2;
class World;

class MainRenderer : public Renderer
{
public:
    MainRenderer();
    virtual ~MainRenderer();
    
    virtual void createDeviceDependentResources();
    virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight);
    virtual void releaseDeviceDependentResources();
    
    virtual void render(int flags = 0);
    
private:
    TextureManager* _textureManager;
    RendererHelper* _rendererHelper;
    SpriteBatcher* _spriteBatcher;
    QuadBatcher* _fillQuadBatcher;
    QuadBatcher* _boundsQuadBatcher;
    LineBatcher* _lineBatcher;
    CircleBatcher* _circleBatcher;
    Box2DDebugRenderer* _box2DDebugRenderer;
    NGShaderLoader* _shaderProgramLoader;
    Font* _font;
    NGRect* _camBounds;
    NGShader* _textureNGShader;
    NGShader* _colorNGShader;
    NGShader* _framebufferToScreenNGShader;
    int _framebufferIndex;
    
    void beginFrame();
    void setFramebuffer(int framebufferIndex);
    void renderBackground();
    void renderWorld(int flags);
    void renderEntities(World* world, bool isServer);
    void renderAtmosphere();
    void renderUI(int engineState);
    void renderMainMenuSteamOffText();
    void renderMainMenuSteamOnText();
    void renderStartingServerText();
    void renderEnterUsernameText();
    void renderJoiningLocalServerByIPText();
    void renderJoiningServerText();
    void renderServerJoinedText(int flags);
    void renderText(const std::string& inStr, const b2Vec2& origin, const Color& inColor, int justification = FONT_ALIGN_LEFT);
    void endFrame();
    
    void updateCamera();
    
    void testRenderingSuite();
};

#endif /* defined(__noctisgames__MainRenderer__) */
