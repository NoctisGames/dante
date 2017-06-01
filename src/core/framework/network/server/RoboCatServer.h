//
//  RoboCatServer.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 5/15/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__RoboCatServer__
#define __noctisgames__RoboCatServer__

#include "RoboCat.h"

class RoboCatServer : public RoboCat
{
    RTTI_DECL;
    
public:
    static GameObject* create();
    
    virtual void handleDying() override;
    
    virtual void update() override;
    
    virtual uint32_t write(OutputMemoryBitStream& inOutputStream, uint32_t inDirtyState) override;
    
protected:
    RoboCatServer();
};

#endif /* defined(__noctisgames__RoboCatServer__) */
