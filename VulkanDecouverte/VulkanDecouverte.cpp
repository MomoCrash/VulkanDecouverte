#include <Windows.h>

#include "framework.h"
#include "GeometryFactory.h"
#include "Mesh.h"
#include "Profiler.h"
#include "RenderObject.h"
#include "RenderWindow.h"
#include "GuiHandler.h"
#include "RenderPipeline.h"
#include "Shader.h"
#include "nodes/NodeEditor.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow)
{

    // Call it to sync this window with the application
    Application::getInstance()->Initialize("Application");
    
    RenderWindow window("Window 1", 500, 500);

    GuiHandler ui;
    int mainWindowIndex = ui.inject(&window);
    
    Profiler profiler;

    Mesh mesh(window, GeometryFactory::GetPrimitive(Primitive::CUBE));

    RenderObject object(&mesh);
    object.setTransform(translate(object.getTransform(), vec3(0.0f, 0.0f, 0.0f)));

    RenderObject object2(&mesh);
    object.setTransform(translate(object.getTransform(), vec3(5.0f, 0.0f, 0.0f)));

    Shader sFragment("frag.spv", Shader::FRAGMENT);
    Shader sVertex("vert.spv", Shader::VERTEX);

    RenderPipeline render({ &sFragment, &sVertex}, window);

    NodeEditor* editor = new NodeEditor(&ui);
    bool isNewWindow = false;

    while (!glfwWindowShouldClose(window.GetWindow())) {

        glfwPollEvents();

        profiler.NewTask("Draw time");

        ///////////////////////////////////// WINDOW 1 ///////////////////////////////
        
        {
            window.update();

            window.clear();
            
            ui.setContext(mainWindowIndex);

            ImGui_ImplVulkan_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();
            
            {
                static float f = 0.0f;
                static int counter = 0;
                static char path[100] = "";

                ImGui::Begin("Creator Content");

                ImGui::Text("Files");

                ImGui::SliderFloat("float", &f, -1.0f, 1.0f);
                if (ImGui::Button("Move forward"))
                {
                    isNewWindow = true;
                }
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);
                ImGui::InputText("Folder", path, sizeof(path));
                if (ImGui::Shortcut(ImGuiKey_Enter))
                {
                    std::cout << path << std::endl;
                }
            
                ImGui::End();
            }
        
            {
                
                static float f = 0.0f;
                static int counter = 0;

                ImGui::Begin("Hello, world! erthe");

                ImGui::Text("This is some useful text.");

                ImGui::SliderFloat("float", &f, -1.0f, 1.0f);
                if (ImGui::Button("Move forward"))
                {
                    object.setTransform(translate(object.getTransform(), vec3(f, 0.0f, f)));
                    counter++;
                }
                ImGui::SameLine();
                ImGui::Text("counter = %d", counter);
            
                ImGui::End();
                
            }

            ImGui::Render();
            ImDrawData* draw_data = ImGui::GetDrawData();
            
            ImGui_ImplVulkan_RenderDrawData(draw_data, window.getCommandBuffer());
            
            window.draw(render, object);
            
            window.display();
        }
        
        ///////////////////////////////////// WINDOW 2 ///////////////////////////////

        if (isNewWindow && !editor->isOpen())
        {
            editor->open();
        }

        isNewWindow = false;
        
        {

            if (editor->isOpen())
            {
                editor->draw();
            }
        }
    }

    // Cleanup
    vkDeviceWaitIdle(Application::getInstance()->getDevice());

    delete editor;

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
}
