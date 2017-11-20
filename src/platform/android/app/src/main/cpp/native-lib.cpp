//
//  native-lib.cpp
//  noctisgames-framework
//
//  Created by Stephen Gowen on 3/18/17.
//  Copyright © 2017 Noctis Games. All rights reserved.
//

#include "framework/input/CursorInputManager.h"
#include "framework/input/KeyboardInputManager.h"
#include "framework/audio/superpowered/android/AndroidAudioEngineHelper.h"
#include "framework/file/android/AndroidAssetDataHandler.h"
#include "framework/graphics/opengl/OpenGLManager.h"
#include "framework/util/FrameworkConstants.h"
#include "framework/input/KeyboardLookup.h"
#include "framework/math/MathUtil.h"

#include "game/logic/MainEngine.h"
#include "game/graphics/portable/MainAssets.h"

#include "GLContext.h"
#include "JNIHelper.h"

#include <jni.h>
#include <errno.h>
#include <chrono>
#include <vector>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/native_window_jni.h>
#include <cpu-features.h>

struct android_app;

class AndroidEngine
{
public:
    static void handleCmd(struct android_app* app, int32_t cmd);
    static int32_t handleInput(android_app* app, AInputEvent* event);
    
    AndroidEngine();
    ~AndroidEngine();
    
    void setState(android_app* state);
    int initDisplay();
    void loadResources();
    void unloadResources();
    void drawFrame();
    void termDisplay();
    void trimMemory();
    bool isReady();
    
    void resume();
    void pause();
    
private:
    ndk_helper::GLContext* m_glContext;
    android_app* m_app;
    MainEngine* m_screen;
    
    float m_fAveragedDeltaTime;
    bool m_hasInitializedResources;
    bool m_hasFocus;
};

void AndroidEngine::handleCmd(struct android_app* app, int32_t cmd)
{
    AndroidEngine* engine = (AndroidEngine*) app->userData;
    switch (cmd)
    {
        case APP_CMD_SAVE_STATE:
            LOGI("NG APP_CMD_SAVE_STATE");
            break;
        case APP_CMD_INIT_WINDOW:
            LOGI("NG APP_CMD_INIT_WINDOW");
            if (app->window != NULL)
            {
                engine->initDisplay();
                engine->drawFrame();
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("NG APP_CMD_TERM_WINDOW");
            engine->termDisplay();
            engine->m_hasFocus = false;
            break;
        case APP_CMD_DESTROY:
            LOGI("NG APP_CMD_DESTROY");
            break;
        case APP_CMD_STOP:
            LOGI("NG APP_CMD_STOP");
            break;
        case APP_CMD_GAINED_FOCUS:
            LOGI("NG APP_CMD_GAINED_FOCUS");
            engine->resume();
            engine->m_hasFocus = true;
            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("NG APP_CMD_LOST_FOCUS");
            engine->pause();
            engine->m_hasFocus = false;
            engine->drawFrame();
            break;
        case APP_CMD_LOW_MEMORY:
            LOGI("NG APP_CMD_LOW_MEMORY");
            engine->trimMemory();
            break;
        case APP_CMD_RESUME:
            LOGI("NG APP_CMD_RESUME");
            engine->resume();
            break;
        case APP_CMD_PAUSE:
            LOGI("NG APP_CMD_PAUSE");
            engine->pause();
            break;
        case APP_CMD_INPUT_CHANGED:
            LOGI("NG APP_CMD_INPUT_CHANGED");
            break;
        case APP_CMD_WINDOW_RESIZED:
            LOGI("NG APP_CMD_WINDOW_RESIZED");
            break;
        case APP_CMD_WINDOW_REDRAW_NEEDED:
            LOGI("NG APP_CMD_WINDOW_REDRAW_NEEDED");
            break;
        case APP_CMD_CONTENT_RECT_CHANGED:
            LOGI("NG APP_CMD_CONTENT_RECT_CHANGED");
            break;
        case APP_CMD_CONFIG_CHANGED:
            LOGI("NG APP_CMD_CONFIG_CHANGED");
            break;
        case APP_CMD_START:
            LOGI("NG APP_CMD_START");
            break;
    }
}

int32_t AndroidEngine::handleInput(android_app* app, AInputEvent* event)
{
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION)
    {
        int32_t action = AMotionEvent_getAction(event);
        int32_t pointerIndex = (action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
        uint32_t flags = action & AMOTION_EVENT_ACTION_MASK;
        
        int32_t pointerCount = AMotionEvent_getPointerCount(event);
        
        switch (flags)
        {
            case AMOTION_EVENT_ACTION_DOWN:
            case AMOTION_EVENT_ACTION_POINTER_DOWN:
            {
                float x = AMotionEvent_getX(event, pointerIndex);
                float y = AMotionEvent_getY(event, pointerIndex);
                CURSOR_INPUT_MANAGER->onTouch(CursorEventType_DOWN, x, y);
            }
                break;
            case AMOTION_EVENT_ACTION_UP:
            case AMOTION_EVENT_ACTION_POINTER_UP:
            case AMOTION_EVENT_ACTION_CANCEL:
            {
                float x = AMotionEvent_getX(event, pointerIndex);
                float y = AMotionEvent_getY(event, pointerIndex);
                CURSOR_INPUT_MANAGER->onTouch(CursorEventType_UP, x, y);
            }
                break;
            case AMOTION_EVENT_ACTION_MOVE:
            {
                for (int i = 0; i < pointerCount; ++i)
                {
                    pointerIndex = i;
                    float x = AMotionEvent_getX(event, pointerIndex);
                    float y = AMotionEvent_getY(event, pointerIndex);
                    CURSOR_INPUT_MANAGER->onTouch(CursorEventType_DRAGGED, x, y);
                }
            }
                break;
        }
        
        return 1;
    }
    else if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_KEY)
    {
        if (AKeyEvent_getKeyCode(event) == AKEYCODE_BACK)
        {
            KEYBOARD_INPUT_MANAGER->onInput(NG_KEY_ESCAPE, true);
            
            return 1; // <-- prevent default handler
        }
    }
    
    return 0;
}

AndroidEngine::AndroidEngine() :
m_glContext(ndk_helper::GLContext::GetInstance()),
m_app(nullptr),
m_screen(nullptr),
m_fAveragedDeltaTime(0.016666666666667f),
m_hasInitializedResources(false),
m_hasFocus(false)
{
    // Empty
}

AndroidEngine::~AndroidEngine()
{
}

void AndroidEngine::setState(android_app* state)
{
    m_app = state;
}

int AndroidEngine::initDisplay()
{
    if (!m_hasInitializedResources)
    {
        m_glContext->Init(m_app->window);
        
        loadResources();
        m_hasInitializedResources = true;
    }
    else
    {
        // initialize OpenGL ES and EGL
        if (EGL_SUCCESS != m_glContext->Resume(m_app->window))
        {
            unloadResources();
            loadResources();
        }
        
        resume();
    }
    
    return 0;
}

void AndroidEngine::loadResources()
{
    JNIEnv *jni;
    m_app->activity->vm->AttachCurrentThread(&jni, NULL);
    
    ANDROID_AUDIO_ENGINE_HELPER->init(jni, m_app->activity->clazz);
    
    AndroidAssetDataHandler::getInstance()->init(jni, m_app->activity->clazz);
    
    MAIN_ASSETS->setUsingCompressedTextureSet(m_glContext->GetScreenWidth() < 2560);
    
    if (!m_screen)
    {
        m_screen = new MainEngine();
    }
    
    int width = m_glContext->GetScreenWidth();
    int height = m_glContext->GetScreenHeight();
    
    m_screen->createDeviceDependentResources();
    
    OGLManager->setScreenSize(width, height);
    
    if (MAIN_ASSETS->isUsingCompressedTextureSet())
    {
        m_screen->createWindowSizeDependentResources(width > 1280 ? 1280 : width, height > 720 ? 720 : height, width, height);
    }
    else
    {
        m_screen->createWindowSizeDependentResources(width > 1440 ? 1440 : width, height > 900 ? 900 : height, width, height);
    }
    
    m_app->activity->vm->DetachCurrentThread();
    return;
}

void AndroidEngine::unloadResources()
{
    ANDROID_AUDIO_ENGINE_HELPER->deinit();
    
    AndroidAssetDataHandler::getInstance()->deinit();
    
    m_screen->releaseDeviceDependentResources();
}

std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();
std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

inline float approxRollingAverage(float avg, float newSample)
{
    static float numSamples = 60;
    
    avg -= avg / numSamples;
    avg += newSample / numSamples;
    
    return avg;
}

void AndroidEngine::drawFrame()
{
    float deltaTime;
    
    using namespace std;
    using namespace std::chrono;
    
    t2 = steady_clock::now();
    duration<float> timeSpan = duration_cast<duration<float>>(t2 - t1);
    t1 = steady_clock::now();
    
    deltaTime = timeSpan.count();
    
    m_fAveragedDeltaTime = approxRollingAverage(m_fAveragedDeltaTime, deltaTime);
    
    deltaTime = clamp(m_fAveragedDeltaTime, 0.016f, 0.033f);
    
    if (m_fAveragedDeltaTime > 0.016f && m_fAveragedDeltaTime < 0.018f)
    {
        deltaTime = 0.016666666666667f;
    }
    
    int requestedAction = m_screen->getRequestedAction();
    
    switch (requestedAction)
    {
        case REQUESTED_ACTION_EXIT:
            // TODO
            break;
        case REQUESTED_ACTION_UPDATE:
            break;
        default:
            m_screen->clearRequestedAction();
            break;
    }
    
    m_screen->update(deltaTime);
    
    m_screen->render();
    
    if (EGL_SUCCESS != m_glContext->Swap())
    {
        unloadResources();
        loadResources();
    }
}

void AndroidEngine::termDisplay()
{
    pause();
    
    m_glContext->Suspend();
}

void AndroidEngine::trimMemory()
{
    m_glContext->Invalidate();
}

bool AndroidEngine::isReady()
{
    if (m_hasFocus)
    {
        return true;
    }
    
    return false;
}

void AndroidEngine::resume()
{
    if (m_screen)
    {
        m_screen->onResume();
        
        m_hasFocus = true;
    }
}

void AndroidEngine::pause()
{
    if (m_screen)
    {
        m_screen->onPause();
        
        m_hasFocus = false;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_m_appglue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(android_app* state)
{
    AndroidEngine engine;
    
    app_dummy();
    
    engine.setState(state);
    
    state->userData = &engine;
    state->onAppCmd = AndroidEngine::handleCmd;
    state->onInputEvent = AndroidEngine::handleInput;
    
#ifdef USE_NDK_PROFILER
    monstartup("native-lib.so");
#endif
    
    while (1)
    {
        // Read all pending events.
        int events;
        android_poll_source* source;
        
        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ALooper_pollAll(engine.isReady() ? 0 : -1, NULL, &events, (void**) &source)) >= 0)
        {
            if (source != NULL)
            {
                source->process(state, source);
            }
            
            if (state->destroyRequested != 0)
            {
                engine.termDisplay();
                return;
            }
        }
        
        if (engine.isReady())
        {
            engine.drawFrame();
        }
    }
}
