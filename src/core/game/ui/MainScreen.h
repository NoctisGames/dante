//
//  MainScreen.h
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#ifndef __dante__MainScreen__
#define __dante__MainScreen__

#include "IScreen.h"

#include "RTTI.h"

#include <thread>

class JsonFile;
class MainRenderer;
class Vector2D;
class PhysicalEntity;

class MainScreen : public IScreen
{
    RTTI_DECL;
    
public:
    MainScreen();
    
    virtual ~MainScreen();
    
    virtual void createDeviceDependentResources();
    
    virtual void createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight);
    
    virtual void releaseDeviceDependentResources();

	virtual void onResume();
	
	virtual void onPause();
    
	virtual void update(float deltaTime);
    
    virtual void render();
    
    int getRequestedAction();
    
    void clearRequestedAction();
    
private:
    JsonFile* m_config;
    MainRenderer* m_renderer;
    Vector2D* m_touchPointDown;
    Vector2D* m_touchPointDown2;
    
    float m_fFrameStateTime;
    int m_iRequestedAction;
    
    // temp
    std::string m_serverAddress;
    std::thread* m_thread1;
    std::thread* m_thread2;
    std::thread* m_thread3;
    PhysicalEntity* m_samus;
    int m_iNetworkAction;
    
    void tempUpdate(float deltaTime);
};

#endif /* defined(__dante__MainScreen__) */