//
//  MainScreen.cpp
//  dante
//
//  Created by Stephen Gowen on 2/22/14.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "pch.h"

#include "MainScreen.h"

#include "JsonFile.h"
#include "MainRenderer.h"
#include "Vector2.h"
#include "PhysicalEntity.h"

#include "GameConstants.h"
#include "ScreenInputManager.h"
#include "ScreenEvent.h"
#include "TouchConverter.h"
#include "KeyboardInputManager.h"
#include "KeyboardEvent.h"
#include "GamePadInputManager.h"
#include "GamePadEvent.h"
#include "NGAudioEngine.h"
#include "StringUtil.h"
#include "MathUtil.h"

#include "RoboCatClient.h"
#include "EntityRegistry.h"
#include "NetworkManagerClient.h"
#include "SocketAddressFactory.h"
#include "SocketUtil.h"
#include "InputManager.h"
#include "Timing.h"
#include "World.h"

MainScreen::MainScreen() : IScreen(),
m_config(new JsonFile("dante.cfg")),
m_renderer(new MainRenderer(MAX_BATCH_SIZE)),
m_touchPointDown(new Vector2()),
m_touchPointDown2(new Vector2()),
m_fStateTime(0),
m_fFrameStateTime(0),
m_iRequestedAction(REQUESTED_ACTION_UPDATE)
{
    m_config->load();
    
    std::string serverIPAddress = m_config->findValue("server_ip");
    if (serverIPAddress.length() == 0)
    {
        //serverAddress = std::string("208.97.168.138:9999");
        serverIPAddress = std::string("localhost:9999");
    }
    
    std::string userID = m_config->findValue("user_id");
    if (userID.length() == 0)
    {
        userID = std::string("Noctis Games");
    }
    
    SocketUtil::staticInit();
    
    World::staticInit();
    
    EntityRegistry::getInstance()->init(World::addEntityIfPossible);
    
    EntityRegistry::getInstance()->registerCreationFunction(NETWORK_TYPE_RoboCat, RoboCatClient::create);
    
    SocketAddress* serverAddress = SocketAddressFactory::CreateIPv4FromString(serverIPAddress);
    
    NetworkManagerClient::getInstance()->init(*serverAddress, userID, World::removeEntityIfPossible);
}

MainScreen::~MainScreen()
{
    delete m_config;
    delete m_renderer;
    delete m_touchPointDown;
    delete m_touchPointDown2;
    
    SocketUtil::CleanUp();
}

void MainScreen::createDeviceDependentResources()
{
	m_renderer->createDeviceDependentResources();
}

void MainScreen::createWindowSizeDependentResources(int renderWidth, int renderHeight, int touchScreenWidth, int touchScreenHeight)
{
	m_renderer->createWindowSizeDependentResources(renderWidth, renderHeight, NUM_FRAMEBUFFERS);

	TOUCH_CONVERTER->setTouchScreenSize(touchScreenWidth, touchScreenHeight);
	TOUCH_CONVERTER->setCamSize(CAM_WIDTH, CAM_HEIGHT);
}

void MainScreen::releaseDeviceDependentResources()
{
	m_renderer->releaseDeviceDependentResources();
}

void MainScreen::onResume()
{
    // TODO
}

void MainScreen::onPause()
{
    NG_AUDIO_ENGINE->pause();
}

void MainScreen::update(float deltaTime)
{
    m_fStateTime += deltaTime;
    m_fFrameStateTime += deltaTime;
    
    Timing::getInstance()->updateManual(m_fStateTime, deltaTime);
    
    if (m_fFrameStateTime >= FRAME_RATE)
    {
        NetworkManagerClient::getInstance()->processIncomingPackets();
        
        tempUpdateInput();
        
        while (m_fFrameStateTime >= FRAME_RATE)
        {
            m_fFrameStateTime -= FRAME_RATE;
            
            tempUpdate(FRAME_RATE);
        }
        
        NG_AUDIO_ENGINE->update();
        
        NetworkManagerClient::getInstance()->sendOutgoingPackets();
    }
}

void MainScreen::render()
{
    m_renderer->beginFrame();
    
    m_renderer->tempDraw(m_fStateTime);
    
    m_renderer->renderToScreen();
    
    m_renderer->endFrame();
}

int MainScreen::getRequestedAction()
{
	return m_iRequestedAction;
}

void MainScreen::clearRequestedAction()
{
    m_iRequestedAction = REQUESTED_ACTION_UPDATE;
}

void MainScreen::tempUpdateInput()
{
    InputManager::getInstance()->update();
}

void MainScreen::tempUpdate(float deltaTime)
{
    World::sInstance->update();
}

RTTI_IMPL(MainScreen, IScreen);
