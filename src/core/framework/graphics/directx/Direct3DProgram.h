//
//  Direct3DProgram.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 2/17/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Direct3DProgram__
#define __noctisgames__Direct3DProgram__

#include "pch.h"

class Direct3DProgram
{
public:
    Direct3DProgram(const char* vertexShaderName, const char* pixelShaderName, bool useTextureCoords);
    
    virtual ~Direct3DProgram();
    
    virtual void mapVertices() = 0;
    
    void bindShaders();
    
    void bindMatrix();

	void createConstantBuffer(_COM_Outptr_opt_  ID3D11Buffer **ppBuffer);
    
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> _vertexShader;
    Microsoft::WRL::ComPtr<ID3D11InputLayout> _inputLayout;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> _pixelShader;
};

#endif /* defined(__noctisgames__Direct3DProgram__) */
