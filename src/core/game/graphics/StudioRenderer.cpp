//
//  StudioRenderer.cpp
//  dante
//
//  Created by Stephen Gowen on 1/4/18.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "game/graphics/StudioRenderer.h"

#include "framework/graphics/portable/TextureManager.h"
#include "framework/graphics/portable/Font.h"
#include "game/logic/World.h"
#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/PolygonBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/NGShaderLoader.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "framework/graphics/portable/NGShader.h"
#include "framework/entity/Entity.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/NGRect.h"
#include "framework/graphics/portable/NGTexture.h"
#include "framework/graphics/portable/Box2DDebugRenderer.h"
#include <game/logic/StudioEngine.h>

#include "framework/graphics/portable/Assets.h"
#include "framework/graphics/portable/RendererHelper.h"
#include "game/logic/GameConstants.h"
#include "framework/math/NGRect.h"
#include "framework/math/Line.h"
#include "framework/graphics/portable/Color.h"
#include "framework/graphics/portable/SpriteBatcher.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/util/macros.h"
#include "framework/network/client/NetworkManagerClient.h"
#include "framework/util/StringUtil.h"
#include "framework/util/WeightedTimedMovingAverage.h"
#include "framework/util/NGSTDUtil.h"
#include "game/logic/InstanceManager.h"
#include "game/logic/TitleInputManager.h"
#include "framework/network/server/NetworkManagerServer.h"
#include "framework/network/portable/MachineAddress.h"
#include "framework/math/MathUtil.h"
#include "framework/audio/portable/NGAudioEngine.h"
#include "game/logic/Server.h"
#include "game/logic/TitleEngine.h"
#include "framework/util/FPSUtil.h"
#include "framework/graphics/portable/Color.h"
#include "framework/math/Circle.h"
#include "framework/graphics/portable/CircleBatcher.h"
#include "framework/graphics/portable/PolygonBatcher.h"
#include "framework/graphics/portable/LineBatcher.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/TextureDataWrapper.h"
#include "framework/graphics/portable/TextureLoaderFactory.h"
#include "framework/graphics/portable/NGShaderLoaderFactory.h"
#include "framework/graphics/portable/RendererHelperFactory.h"
#include "framework/util/NGSTDUtil.h"
#include "framework/math/Circle.h"
#include "framework/math/Line.h"
#include <framework/graphics/portable/NGTextureShader.h>
#include <framework/graphics/portable/NGGeometryShader.h>
#include <framework/graphics/portable/NGFramebufferToScreenShader.h>
#include "framework/graphics/portable/NGTextureDesc.h"
#include "framework/graphics/portable/Assets.h"
#include "framework/input/CursorInputManager.h"
#include "framework/input/CursorConverter.h"

#ifdef NG_STEAM
#include "framework/network/steam/NGSteamGameServer.h"
#include "framework/network/steam/NGSteamGameServices.h"
#endif

#include <sstream>
#include <ctime> // rand
#include <string>
#include <assert.h>
#include <cfloat>

StudioRenderer::StudioRenderer() : Renderer(),
_textureManager(new TextureManager("game_assets.cfg")),
_rendererHelper(RENDERER_HELPER_FACTORY->createRendererHelper()),
_fillPolygonBatcher(new PolygonBatcher(_rendererHelper, true)),
_boundsPolygonBatcher(new PolygonBatcher(_rendererHelper, false)),
_lineBatcher(new LineBatcher(_rendererHelper)),
_circleBatcher(new CircleBatcher(_rendererHelper)),
_box2DDebugRenderer(new Box2DDebugRenderer(*_fillPolygonBatcher, *_boundsPolygonBatcher, *_lineBatcher, *_circleBatcher)),
_shaderProgramLoader(SHADER_PROGRAM_LOADER_FACTORY->createNGShaderLoader()),
_textureNGShader(new NGTextureShader(*_rendererHelper, "shader_003_vert.ngs", "shader_003_frag.ngs")),
_colorNGShader(new NGGeometryShader(*_rendererHelper, "shader_001_vert.ngs", "shader_001_frag.ngs")),
_framebufferToScreenNGShader(new NGFramebufferToScreenShader(*_rendererHelper, "shader_002_vert.ngs", "shader_002_frag.ngs")),
_font(new Font("texture_001.ngt", 0, 0, 16, 64, 75, 1024, 1024)),
_toastStateTime(0),
_fbIndex(0),
_scrollValue(1)
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i] = new SpriteBatcher(_rendererHelper);
    }
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i] = new NGRect(0, 0, CAM_WIDTH, CAM_HEIGHT);
    }
}

StudioRenderer::~StudioRenderer()
{
    releaseDeviceDependentResources();

    delete _textureManager;
    delete _rendererHelper;
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        delete _spriteBatchers[i];
    }
    delete _fillPolygonBatcher;
    delete _boundsPolygonBatcher;
    delete _lineBatcher;
    delete _circleBatcher;
    delete _shaderProgramLoader;
    delete _font;
    delete _textureNGShader;
    delete _colorNGShader;
    delete _framebufferToScreenNGShader;
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        delete _camBounds[i];
    }
}

void StudioRenderer::createDeviceDependentResources()
{
    _rendererHelper->createDeviceDependentResources();
    _textureManager->createDeviceDependentResources();

    _textureNGShader->load(*_shaderProgramLoader);
    _colorNGShader->load(*_shaderProgramLoader);
    _framebufferToScreenNGShader->load(*_shaderProgramLoader);
}

void StudioRenderer::createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight)
{
    _rendererHelper->createWindowSizeDependentResources(screenWidth, screenHeight, renderWidth, renderHeight);
}

void StudioRenderer::releaseDeviceDependentResources()
{
    _rendererHelper->releaseDeviceDependentResources();
    _textureManager->releaseDeviceDependentResources();

    _textureNGShader->unload(*_shaderProgramLoader);
    _colorNGShader->unload(*_shaderProgramLoader);
    _framebufferToScreenNGShader->unload(*_shaderProgramLoader);
}

void StudioRenderer::render(int flags)
{
    setFramebuffer(0, 0, 0, 0, 1);
    _rendererHelper->useNormalBlending();

    if (_textureManager->ensureTextures())
    {
        renderWorld();
        
        if (flags & StudioEngineState_DisplayBox2D)
        {
            renderBox2D();
        }
        
        if (flags & StudioEngineState_DisplayGrid)
        {
            renderGrid();
        }
        
        renderUI();
    }

    endFrame();
}

void StudioRenderer::update(float x, float y, float w, float h, int scale)
{
    _scrollValue = scale;
    
    _camBounds[3]->getLowerLeft().set(x, y);
    _camBounds[2]->getLowerLeft().set(x / 2, y / 2);
    _camBounds[1]->getLowerLeft().set(x / 4, y / 4);
    _camBounds[0]->getLowerLeft().set(x / 8, y / 8);
    
    for (int i = 0; i < NUM_CAMERAS; ++i)
    {
        _camBounds[i]->setWidth(w);
        _camBounds[i]->setHeight(h);
    }
    
    if (_toasts.size() > 0)
    {
        ++_toastStateTime;
        if (_toastStateTime >= 60)
        {
            _toasts.pop_front();
            _toastStateTime = 0;
        }
    }
}

void StudioRenderer::setWorld(World* inValue)
{
    _world = inValue;
}

void StudioRenderer::displayToast(std::string toast)
{
    _toasts.push_back(toast);
    _toastStateTime = 0;
}

void StudioRenderer::setFramebuffer(int framebufferIndex, float r, float g, float b, float a)
{
    assert(framebufferIndex >= 0);

    _fbIndex = framebufferIndex;

    _rendererHelper->bindToOffscreenFramebuffer(_fbIndex);
    _rendererHelper->clearFramebufferWithColor(r, g, b, a);
}

void StudioRenderer::renderWorld()
{
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    
    setFramebuffer(0);
    _rendererHelper->useNormalBlending();
    renderLayers();
    
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    setFramebuffer(1);
    _rendererHelper->useScreenBlending();
    renderEntities();
    
    setFramebuffer(2);
    
    _rendererHelper->useScreenBlending();
    for (int i = 0; i < _fbIndex; ++i)
    {
        static std::vector<VERTEX_2D> screenVertices;
        screenVertices.clear();
        screenVertices.reserve(4);
        screenVertices.push_back(VERTEX_2D(-1, -1));
        screenVertices.push_back(VERTEX_2D(-1, 1));
        screenVertices.push_back(VERTEX_2D(1, 1));
        screenVertices.push_back(VERTEX_2D(1, -1));
        
        _framebufferToScreenNGShader->bind(&screenVertices, _rendererHelper->getFramebuffer(i));
        _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
        _framebufferToScreenNGShader->unbind();
    }
}

void StudioRenderer::renderLayers()
{
    for (int i = 0; i < 3; ++i)
    {
        _spriteBatchers[i]->beginBatch();
    }
    
    std::string textures[3];
    
    std::vector<Entity*> entities = _world->getLayers();
    for (Entity* e : entities)
    {
        TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
        
        _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
        textures[e->getEntityDef().layer] = tr.getTextureName();
    }
    
    for (int i = 0; i < 3; ++i)
    {
        if (textures[i].length() > 0)
        {
            _rendererHelper->updateMatrix(_camBounds[i]->getLeft(), _camBounds[i]->getRight(), _camBounds[i]->getBottom(), _camBounds[i]->getTop());
            _spriteBatchers[i]->endBatch(_textureNGShader, _textureManager->getTextureWithName(textures[i]));
        }
    }
}

void StudioRenderer::renderEntities()
{
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        _spriteBatchers[i]->beginBatch();
    }
    
    std::string textures[NUM_SPRITE_BATCHERS];
    
    {
        std::vector<Entity*> entities = _world->getPlayers();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = tr.getTextureName();
        }
    }
    
    {
        std::vector<Entity*> entities = _world->getDynamicEntities();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = tr.getTextureName();
        }
    }
    
    {
        std::vector<Entity*> entities = _world->getStaticEntities();
        for (Entity* e : entities)
        {
            TextureRegion tr = ASSETS->findTextureRegion(e->getTextureMapping(), e->getStateTime());
            
            _spriteBatchers[e->getEntityDef().layer]->renderSprite(e->getPosition().x, e->getPosition().y, e->getWidth(), e->getHeight(), e->getAngle(), tr, e->isFacingLeft());
            textures[e->getEntityDef().layer] = tr.getTextureName();
        }
    }
    
    for (int i = 0; i < NUM_SPRITE_BATCHERS; ++i)
    {
        if (textures[i].length() > 0)
        {
            _spriteBatchers[i]->endBatch(_textureNGShader, _textureManager->getTextureWithName(textures[i]));
        }
    }
}

void StudioRenderer::renderBox2D()
{
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    
    _box2DDebugRenderer->render(&_world->getWorld(), _colorNGShader);
}

void StudioRenderer::renderGrid()
{
    int left = clamp(_camBounds[3]->getLeft(), FLT_MAX, 0);
    int bottom = clamp(_camBounds[3]->getBottom(), FLT_MAX, 0);
    int camWidth = _camBounds[3]->getRight() + 1;
    int camHeight = _camBounds[3]->getTop() + 1;
    
    Color lineColor = Color::WHITE;
    lineColor.alpha = 0.25f;
    
    _rendererHelper->useNormalBlending();
    _rendererHelper->updateMatrix(_camBounds[3]->getLeft(), _camBounds[3]->getRight(), _camBounds[3]->getBottom(), _camBounds[3]->getTop());
    
    _lineBatcher->beginBatch();
    for (int i = left; i <= camWidth; ++i)
    {
        _lineBatcher->renderLine(i, 0, i, camHeight);
    }
    for (int i = bottom; i <= camHeight; ++i)
    {
        _lineBatcher->renderLine(0, i, camWidth, i);
    }
    _lineBatcher->endBatch(_colorNGShader, lineColor);
    
    _lineBatcher->beginBatch();
    _lineBatcher->renderLine(0, 0, 0, camHeight);
    _lineBatcher->renderLine(0, 0, camWidth, 0);
    _lineBatcher->endBatch(_colorNGShader, Color::RED);
}

void StudioRenderer::renderUI()
{
    _rendererHelper->updateMatrix(0, CAM_WIDTH, 0, CAM_HEIGHT);
    
    int y = CAM_HEIGHT - 2;
    _spriteBatchers[0]->beginBatch();
    for (std::string t : _toasts)
    {
        renderText(t.c_str(), CAM_WIDTH / 2, y, Color::WHITE, FONT_ALIGN_CENTERED);
        y -= 2;
    }
    _spriteBatchers[0]->endBatch(_textureNGShader, _textureManager->getTextureWithName("texture_000.ngt"));
}

void StudioRenderer::renderText(const char* inStr, float x, float y, const Color& inColor, int justification)
{
    Color fontColor = Color(inColor.red, inColor.green, inColor.blue, inColor.alpha);
    float fgWidth = CAM_WIDTH / 60;
    float fgHeight = fgWidth * 1.171875f;

    std::string text(inStr);

    _font->renderText(*_spriteBatchers[0], text, x, y, fgWidth, fgHeight, fontColor, justification);
}

void StudioRenderer::endFrame()
{
    assert(_fbIndex >= 0);

    _rendererHelper->bindToScreenFramebuffer();
    _rendererHelper->clearFramebufferWithColor(0, 0, 0, 1);
    _rendererHelper->useScreenBlending();

    static std::vector<VERTEX_2D> screenVertices;
    screenVertices.clear();
    screenVertices.push_back(VERTEX_2D(-1, -1));
    screenVertices.push_back(VERTEX_2D(-1, 1));
    screenVertices.push_back(VERTEX_2D(1, 1));
    screenVertices.push_back(VERTEX_2D(1, -1));

    _framebufferToScreenNGShader->bind(&screenVertices, _rendererHelper->getFramebuffer(_fbIndex));
    _rendererHelper->drawIndexed(NGPrimitiveType_Triangles, 0, INDICES_PER_RECTANGLE);
    _framebufferToScreenNGShader->unbind();

    _rendererHelper->useNoBlending();
}
