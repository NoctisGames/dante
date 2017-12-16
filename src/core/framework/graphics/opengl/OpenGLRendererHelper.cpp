//
//  OpenGLRendererHelper.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "framework/graphics/opengl/OpenGLRendererHelper.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/graphics/opengl/OpenGLManager.h"

OpenGLRendererHelper::OpenGLRendererHelper() : RendererHelper()
{
    OpenGLManager::create();
}

OpenGLRendererHelper::~OpenGLRendererHelper()
{
    OpenGLManager::destroy();
}

void OpenGLRendererHelper::createDeviceDependentResources(int maxBatchSize)
{
    OGLManager->createDeviceDependentResources(maxBatchSize);
}

void OpenGLRendererHelper::createWindowSizeDependentResources(int renderWidth, int renderHeight, int numFramebuffers)
{
    OGLManager->createWindowSizeDependentResources(renderWidth, renderHeight, numFramebuffers);
}

void OpenGLRendererHelper::releaseDeviceDependentResources()
{
    OGLManager->releaseDeviceDependentResources();
}

void OpenGLRendererHelper::beginFrame()
{
    glEnable(GL_TEXTURE_2D);
    
    glEnable(GL_BLEND);
}

void OpenGLRendererHelper::endFrame()
{
    glDisable(GL_BLEND);
    
    glDisable(GL_TEXTURE_2D);
}

TextureWrapper* OpenGLRendererHelper::getFramebuffer(int index)
{
    _framebuffer->gpuTextureWrapper = OGLManager->getFramebuffers().at(index);
    
    return _framebuffer;
}

void OpenGLRendererHelper::updateMatrix(float left, float right, float bottom, float top)
{
    OGLManager->createMatrix(left, right, bottom, top);
}

void OpenGLRendererHelper::bindToOffscreenFramebuffer(int index)
{
    glBindFramebuffer(GL_FRAMEBUFFER, OGLManager->getFbos().at(index));
    
    int width = OGLManager->getRenderWidth();
    int height = OGLManager->getRenderHeight();
    
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);
}

void OpenGLRendererHelper::clearFramebufferWithColor(float r, float g, float b, float a)
{
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

void OpenGLRendererHelper::bindToScreenFramebuffer()
{
    glBindFramebuffer(GL_FRAMEBUFFER, OGLManager->getScreenFBO());
    
    int width = OGLManager->getScreenWidth();
    int height = OGLManager->getScreenHeight();
    
    glViewport(0, 0, width, height);
    glScissor(0, 0, width, height);
}

void OpenGLRendererHelper::destroyTexture(GpuTextureWrapper& textureWrapper)
{
    glDeleteTextures(1, &textureWrapper.texture);
}

void OpenGLRendererHelper::clearColorVertices()
{
    OGLManager->getColorVertices().clear();
}

void OpenGLRendererHelper::clearTextureVertices()
{
    OGLManager->getTextureVertices().clear();
}

void OpenGLRendererHelper::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v)
{
    OGLManager->addVertexCoordinate(x, y, z, r, g, b, a, u, v);
}

void OpenGLRendererHelper::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a)
{
    OGLManager->addVertexCoordinate(x, y, z, r, g, b, a);
}

void OpenGLRendererHelper::draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count)
{
    glDrawArrays(renderPrimitiveType, first, count);
}

void OpenGLRendererHelper::drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count)
{
    glDrawElements(renderPrimitiveType, count, GL_UNSIGNED_SHORT, &OGLManager->getIndices()[0]);
}

void OpenGLRendererHelper::bindTexture(NGTextureSlot textureSlot, TextureWrapper* textureWrapper)
{
    glActiveTexture(textureSlot);
    glBindTexture(GL_TEXTURE_2D, textureWrapper == NULL ? 0 : textureWrapper->gpuTextureWrapper->texture);
}
