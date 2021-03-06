//
//  NGShader.h
//  noctisgames
//
//  Created by Stephen Gowen on 8/27/15.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__NGShader__
#define __noctisgames__NGShader__

#include <cstddef>
#include <vector>
#include <string>

class RendererHelper;
class ShaderProgramLoader;
class NGShaderUniformInput;
class NGShaderVarInput;
struct ShaderProgramWrapper;

class NGShader
{
public:
    NGShader(RendererHelper& inRendererHelper, const char* vertexShaderName, const char* fragmentShaderName);
    virtual ~NGShader();
    
    virtual void bind(void* data1 = NULL, void* data2 = NULL, void* data3 = NULL) = 0;
    virtual void unbind() = 0;
    
    void load(ShaderProgramLoader& shaderProgramLoader);
    void unload(ShaderProgramLoader& shaderProgramLoader);
    bool isLoaded();
    
protected:
    RendererHelper& _rendererHelper;
    ShaderProgramWrapper* _shaderProgramWrapper;
    std::vector<NGShaderUniformInput*> _uniforms;
    std::vector<NGShaderVarInput*> _inputLayout;
    std::string _vertexShaderFilePath;
    std::string _fragmentShaderFilePath;
};

#endif /* defined(__noctisgames__NGShader__) */
