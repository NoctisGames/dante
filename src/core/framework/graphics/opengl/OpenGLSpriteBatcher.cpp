//
//  OpenGLSpriteBatcher.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "framework/graphics/opengl/OpenGLSpriteBatcher.h"

#include "framework/graphics/portable/TextureWrapper.h"
#include "framework/graphics/portable/GpuTextureWrapper.h"
#include "framework/graphics/portable/GpuProgramWrapper.h"
#include "framework/graphics/portable/TextureRegion.h"
#include "framework/math/Color.h"

#include "framework/graphics/opengl/OpenGLManager.h"

OpenGLSpriteBatcher::OpenGLSpriteBatcher()
{
    _numSprites = 0;
}

void OpenGLSpriteBatcher::beginBatch()
{
    OGLManager->getTextureVertices().clear();
    
    _numSprites = 0;
}

void OpenGLSpriteBatcher::endBatch(TextureWrapper* textureWrapper, GpuProgramWrapper& gpuProgramWrapper)
{
    if (_numSprites > 0)
    {
        // tell the GPU which texture to use
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureWrapper->gpuTextureWrapper->texture);
        
        gpuProgramWrapper.bind();
        
        glDrawElements(GL_TRIANGLES, _numSprites * INDICES_PER_RECTANGLE, GL_UNSIGNED_SHORT, &OGLManager->getIndices()[0]);
        
        gpuProgramWrapper.unbind();
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void OpenGLSpriteBatcher::addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v)
{
    OGLManager->addVertexCoordinate(x, y, z, r, g, b, a, u, v);
}
