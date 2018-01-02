//
//  DirectXProgramLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__DirectXProgramLoader__
#define __noctisgames__DirectXProgramLoader__

#include <framework/graphics/portable/NGShaderLoader.h>

#include "framework/graphics/portable/NGGraphics.h"

class DirectXProgramLoader : public NGShaderLoader
{
public:
    DirectXProgramLoader();
    
    virtual ~DirectXProgramLoader();
    
    virtual ShaderProgramWrapper* loadNGShader(const char* vertexShaderName, const char* fragmentShaderName, std::vector<NGShaderUniformInput*>& uniforms, std::vector<NGShaderVarInput*>& inputLayout);
};

#endif /* defined(__noctisgames__DirectXProgramLoader__) */
