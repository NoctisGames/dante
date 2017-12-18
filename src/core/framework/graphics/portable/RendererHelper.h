//
//  RendererHelper.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 1/15/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RendererHelper__
#define __noctisgames__RendererHelper__

#include <framework/graphics/portable/NGPrimitiveType.h>
#include <framework/graphics/portable/NGTextureSlot.h>
#include <framework/graphics/portable/VertexProgramInput.h>

#include <vector>

class NGTexture;
struct TextureWrapper;
class ShaderProgramWrapper;

class RendererHelper
{
public:
    RendererHelper();
    
    virtual ~RendererHelper();

	virtual void createDeviceDependentResources() = 0;

	virtual void createWindowSizeDependentResources(int screenWidth, int screenHeight, int renderWidth, int renderHeight, int numFramebuffers) = 0;

	virtual void releaseDeviceDependentResources() = 0;
    
    virtual void beginFrame() = 0;
    
    virtual void endFrame() = 0;
    
    virtual NGTexture* getFramebuffer(int index) = 0;
    
    virtual void updateMatrix(float left, float right, float bottom, float top) = 0;
    
    virtual void bindToOffscreenFramebuffer(int index) = 0;
    
    virtual void clearFramebufferWithColor(float r, float g, float b, float a) = 0;
    
    virtual void bindToScreenFramebuffer() = 0;
    
    virtual void useNormalBlending() = 0;
    virtual void useScreenBlending() = 0;
    
    virtual void bindMatrix(int32_t location = 0) = 0;
    
    virtual void bindTexture(NGTextureSlot textureSlot, NGTexture* texture, int32_t location = 0) = 0;
    virtual void destroyTexture(TextureWrapper& textureWrapper) = 0;
    
    virtual void bindShaderProgram(ShaderProgramWrapper* shaderProgramWrapper) = 0;
    virtual void destroyShaderProgram(ShaderProgramWrapper* shaderProgramWrapper) = 0;
    
    virtual void mapTextureVertices() = 0;
    virtual void unmapTextureVertices() = 0;
    
    virtual void mapColorVertices() = 0;
    virtual void unmapColorVertices() = 0;
    
    virtual void draw(NGPrimitiveType renderPrimitiveType, uint32_t first, uint32_t count) = 0;
    virtual void drawIndexed(NGPrimitiveType renderPrimitiveType, uint32_t count) = 0;
    
#if defined __APPLE__ || defined __ANDROID__ || defined __linux__
    virtual GLuint getTextureVertexBuffer() = 0;
    virtual GLuint getColorVertexBuffer() = 0;
    virtual mat4x4& getMatrix() = 0;
#elif defined _WIN32
    virtual Microsoft::WRL::ComPtr<ID3D11Buffer>& getTextureVertexBuffer() = 0;
    virtual Microsoft::WRL::ComPtr<ID3D11Buffer>& getColorVertexBuffer() = 0;
    virtual DirectX::XMFLOAT4X4& getMatrix() = 0;
#endif
    
    void clearColorVertices();
    void clearTextureVertices();
    
    void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a, float u, float v);
    
    void addVertexCoordinate(float x, float y, float z, float r, float g, float b, float a);
    
    std::vector<TEXTURE_VERTEX>& getTextureVertices();
    std::vector<COLOR_VERTEX>& getColorVertices();
    
protected:
    NGTexture* _framebuffer;
    
    std::vector<TextureWrapper *> _framebuffers;
    std::vector<TEXTURE_VERTEX> _textureVertices;
    std::vector<COLOR_VERTEX> _colorVertices;
    std::vector<uint16_t> _indices;
    
    int _screenWidth;
    int _screenHeight;
    int _renderWidth;
    int _renderHeight;
    int _numFramebuffers;
    
    virtual void createFramebufferObject() = 0;
    virtual void releaseFramebuffers() = 0;
    
    void createFramebufferObjects();
    
private:
    void generateIndices(int maxBatchSize);
};

#endif /* defined(__noctisgames__RendererHelper__) */
