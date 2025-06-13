#include "NodeEditor.h"

#include "node.hpp"

NodeEditor::NodeEditor(GuiHandler* handler)
    : BaseNode(), window(nullptr)
{
    contextGuiHandlers = handler;
}

NodeEditor::~NodeEditor()
{
    if (isOpen())
        delete window;
}

void NodeEditor::open()
{
    window = new RenderWindow("Editor Window", 500, 500);
    index = contextGuiHandlers->inject(window);

    mINF.addNode<SimpleSum>(ImVec2(200, 200));
}

void NodeEditor::draw()
{

    if (glfwWindowShouldClose(window->GetWindow()))
    {
        vkDeviceWaitIdle(Application::getInstance()->getDevice());
        delete window;
        window = nullptr;
        contextGuiHandlers->remove(index);
        return;
    }

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
            
    ImGui_ImplVulkan_RenderDrawData(draw_data, window->getCommandBuffer());

    window->display();
        
}

bool NodeEditor::isOpen()
{
    return window != nullptr;
}