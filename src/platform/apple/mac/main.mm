//
//  main.mm
//  dante
//
//  Created by Stephen Gowen on 11/16/17.
//  Copyright (c) 2017 Noctis Games. All rights reserved.
//

#include "MainEngine.h"
#include "CursorInputManager.h"
#include "KeyboardInputManager.h"
#include "MainAssets.h"
#include "FrameworkConstants.h"
#include "OpenGLManager.h"

#include "NGGraphics.h"

#include <glfw/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

static int joysticks[GLFW_JOYSTICK_LAST + 1];
static int joystick_count = 0;

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}

static void joystick_callback(int jid, int event)
{
    if (event == GLFW_CONNECTED)
    {
        joysticks[joystick_count++] = jid;
    }
    else if (event == GLFW_DISCONNECTED)
    {
        int i;
        
        for (i = 0; i < joystick_count; ++i)
        {
            if (joysticks[i] == jid)
            {
                break;
            }
        }
        
        for (i = i + 1; i < joystick_count; ++i)
        {
            joysticks[i - 1] = joysticks[i];
        }
        
        joystick_count--;
    }
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    static double cursorX;
    static double cursorY;
    
    glfwGetCursorPos(window, &cursorX, &cursorY);
    
    switch (action)
    {
        case GLFW_PRESS:
            SCREEN_INPUT_MANAGER->onTouch(CursorEventType_DOWN, cursorX, cursorY);
            break;
        case GLFW_REPEAT:
            SCREEN_INPUT_MANAGER->onTouch(CursorEventType_DRAGGED, cursorX, cursorY);
            break;
        case GLFW_RELEASE:
            SCREEN_INPUT_MANAGER->onTouch(CursorEventType_UP, cursorX, cursorY);
            break;
        default:
            break;
    }
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    KEYBOARD_INPUT_MANAGER->onInput(key, action == GLFW_RELEASE);
}

int main(void)
{
    GLFWwindow* window;
    
    glfwSetErrorCallback(error_callback);
    
    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    
    GLFWmonitor* monitor = NULL;
    
    int width = 800;
    int height = 480;
    
#ifdef NG_LAUNCH_AS_FULL_SCREEN
    monitor = glfwGetPrimaryMonitor();
    
    if (monitor)
    {
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
        
        width  = mode->width;
        height = mode->height;
    }
#endif
    
    int jid, hat_buttons = GLFW_FALSE;
    
    memset(joysticks, 0, sizeof(joysticks));
    
    window = glfwCreateWindow(width, height, "Project Dante", monitor, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    
    for (jid = GLFW_JOYSTICK_1;  jid <= GLFW_JOYSTICK_LAST;  jid++)
    {
        if (glfwJoystickPresent(jid))
        {
            joysticks[joystick_count++] = jid;
        }
    }
    
    glfwSetJoystickCallback(joystick_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    
    MAIN_ASSETS->setUsingDesktopTextureSet(true);
    
    MainEngine engine;
    float lastTime = 0;
    engine.createDeviceDependentResources();
    
    int glWidth = 0, glHeight = 0;
    
    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &width, &height);
        
        if (width != glWidth || height != glHeight)
        {
            OGLManager->setScreenSize(width, height);
            
            engine.createWindowSizeDependentResources(width > 1440 ? 1440 : width, height > 900 ? 900 : height, width, height);
            
            glWidth = width;
            glHeight = height;
        }
        
        
        
        double time = glfwGetTime();
        
        double deltaTime = time - lastTime;
        lastTime = time;
        
        int requestedAction = engine.getRequestedAction();
        
        switch (requestedAction)
        {
            case REQUESTED_ACTION_EXIT:
                glfwSetWindowShouldClose(window, GLFW_TRUE);
                break;
            case REQUESTED_ACTION_UPDATE:
                break;
            default:
                engine.clearRequestedAction();
                break;
        }
        
        engine.update(deltaTime);
        engine.render();
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwDestroyWindow(window);
    
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
