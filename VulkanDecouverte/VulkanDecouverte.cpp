#include <Windows.h>

#include "framework.h"
#include "Mesh.h"
#include "Profiler.h"
#include "RenderObject.h"
#include "RenderWindow.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, int nCmdShow) 
{

    RenderWindow window("Salutation", 800, 600);
    //RenderWindow window2("Salutation", 800, 600);
    Profiler profiler;

    Mesh mesh(window, MeshData::Cube());

    RenderObject object(&mesh);
    Shader shader(window, "shaders/vert.spv", "shaders/frag.spv");
    
    mat4 model = mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.0f, 1.0f, 0.0f));
    
    mat4 model2 = mat4(1.0f);
    model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f));

    while (!glfwWindowShouldClose(window.GetWindow())) {

        profiler.NewTask("Draw time");
        window.update();
        
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");

            ImGui::Text("This is some useful text.");

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            if (ImGui::Button("Button"))
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);
            
            ImGui::End();
        }
        
        ImGui::Render();
        ImDrawData* draw_data = ImGui::GetDrawData();

        window.clear();
        
        ImGui_ImplVulkan_RenderDrawData(draw_data, window.getCommandBuffer());
        //window.draw(object, shader, model2);

        window.display();
        
        profiler.EndTask();
        

    }

    // Cleanup
    vkDeviceWaitIdle(window.getDevice());

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
}