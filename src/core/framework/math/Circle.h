//
//  Circle.h
//  noctisgames-framework
//
//  Created by Stephen Gowen on 8/6/13.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __noctisgames__Circle__
#define __noctisgames__Circle__

#include "Vector2.h"

class Circle
{
public:
	Circle(float x, float y, float radius);
    
	Vector2& getCenter();
    
    float getRadius();

private:
    Vector2 m_center;
    float m_fRadius;
};

#endif /* defined(__noctisgames__Circle__) */
