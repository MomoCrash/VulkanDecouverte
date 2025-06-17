#include "Editor.h"

#include "../GeometryFactory.h"
#include "../Mesh.h"
#include "../RenderObject.h"
#include "../Shader.h"
#include "../RenderPipeline.h"
#include "../nodes/NodeEditor.h"

Editor::Editor(GuiHandler* guiHandler) : RenderWindow("SVE", 800, 800)
{
        
    m_mainWindowContext = guiHandler->inject(this);

    Shader sFragment("frag.spv", Shader::FRAGMENT);
    Shader sVertex("vert.spv", Shader::VERTEX);
    
    m_renderPipeline = new RenderPipeline({ &sFragment, &sVertex}, *this);
    
    m_nodeEditor = new NodeEditor(guiHandler);
    m_guiHandler = guiHandler;
    
}

Editor::~Editor()
{
    delete m_renderPipeline;
    delete m_nodeEditor;
}

void Editor::draw()
{
    
    update();

    clear();

    m_guiHandler->setContext(m_mainWindowContext);
    
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    
    {
        
        static float f = 0.0f;
        static int counter = 0;
        static bool isWindowOpen = false;

        // Draw the GUI
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus ;
        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        if (!isWindowOpen)
        {
            ImGui::SetNextWindowPos(viewport->Pos);
            ImGui::SetNextWindowSize(viewport->Size);
        }
        if (ImGui::Begin("##Fullscreen window", &isWindowOpen, flags))
        {
            isWindowOpen = true;
            ImGui::Text("This is some useful text.");

            ImGui::SliderFloat("float", &f, -1.0f, 1.0f);
            if (ImGui::Button("Move forward")) {
                m_nodeEditor->open();
            }
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::End();
            
        }

        // Draw the GUI
        ImGui::Begin("Scene", nullptr);

        ImGui::Text("This is some useful text.");

        ImGui::SliderFloat("float", &f, -1.0f, 1.0f);
        if (ImGui::Button("Move forward")) {
            m_nodeEditor->open();
        }
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);
    
        ImGui::End();
        
    }

    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
    
    ImGui_ImplVulkan_RenderDrawData(draw_data, getCommandBuffer());
    
    display();

    m_nodeEditor->draw();
    
}
