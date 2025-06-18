#define STB_IMAGE_IMPLEMENTATION
#include "libs/stb_image.h"

#include <Windows.h>

#include "Application.h"
#include "framework.h"

#include "GeometryFactory.h"

#include "RenderWindow.h"
#include "GuiHandler.h"
#include "Mesh.h"
#include "RenderObject.h"
#include "RenderPipeline.h"
#include "Shader.h"
#include "editor/Editor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{

    // Call it to sync this window with the application
    Application::getInstance()->Initialize("Application");
    
    GuiHandler ui;

    Editor editor(&ui);

    // VkFormat format = Application::getInstance()->findSupportedFormat(
    // {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
    // VK_IMAGE_TILING_OPTIMAL,
    // VK_FORMAT_FEATURE_SAMPLED_IMAGE_BIT );

    while(!editor.shouldClose())
    {
        
        glfwPollEvents();
        
        editor.draw();
        
    }

    vkDeviceWaitIdle(Application::getInstance()->getDevice());
    
    return 0;
    
}
