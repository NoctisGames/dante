//
//  ShaderProgramLoader.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 12/17/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__ShaderProgramLoader__
#define __noctisgames__ShaderProgramLoader__

struct ShaderProgramWrapper;

class ShaderProgramLoader
{
public:
    ShaderProgramLoader();
    
    virtual ~ShaderProgramLoader();
    
    virtual ShaderProgramWrapper* loadShaderProgram(const char* vertexShaderName, const char* fragmentShaderName) = 0;
};

#endif /* defined(__noctisgames__ShaderProgramLoader__) */
