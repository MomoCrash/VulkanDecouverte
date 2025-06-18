#include "NodeEditor.h"

#include "node.hpp"

NodeEditor::NodeEditor(GuiHandler* handler)
    : BaseNode(), window(nullptr)
{
    contextGuiHandlers = handler;
}

NodeEditor::~NodeEditor()
{
    if (window != nullptr)
        delete window;
}

void NodeEditor::open()
{
    m_isOpen = true;
}

void NodeEditor::draw()
{

    if (m_isOpen)
    {
        window = new RenderWindow("Editor Window", 500, 500);
        index = contextGuiHandlers->inject(window);

        mINF.addNode<SimpleSum>(ImVec2(200, 200));
        m_isOpen = false;
    }
    
    if (window == nullptr) return;

    window->update();
    window->clear();

    contextGuiHandlers->setContext(index);
    
    // Init the ImGUI Frame
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Set content size on window size
    ImGui::SetNextWindowSize(ImVec2(window->getExtent2D().width, window->getExtent2D().height));
    ImGui::SetNextWindowPos(ImVec2(1, 1));

    // Draw the GUI
    ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
    mINF.update();
    BaseNode::draw();
    ImGui::End();

    // Do the final render
    ImGui::Render();
    ImDrawData* draw_data = ImGui::GetDrawData();
            
    ImGui_ImplVulkan_RenderDrawData(draw_data, window->getRenderContext()->getCommandBuffer(window->getCurrentFrame()));

    window->display();

    if (glfwWindowShouldClose(window->GetWindow()))
    {
        window->clear();
        
        delete window;
        window = nullptr;
        contextGuiHandlers->setContext(0);
        contextGuiHandlers->remove(index);
        
    }
        
}
