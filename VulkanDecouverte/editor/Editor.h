#pragma once

#include "InspectorWindow.h"
#include "../GuiHandler.h"
#include "../RenderWindow.h"

class Mesh;
struct NodeEditor;

class Editor final : public RenderWindow
{
public:
    Editor(GuiHandler* guiHandler);
    ~Editor() override;
    
    void draw();

private:
    InspectorWindow m_inspectorWindow;
    RenderObject* m_testObject;
    Mesh* m_mesh;

    VkDescriptorSet DS[2];
    
    RenderPipeline* m_renderPipeline;
    NodeEditor*     m_nodeEditor;

    GuiHandler*     m_guiHandler;
    int m_mainWindowContext;
};
