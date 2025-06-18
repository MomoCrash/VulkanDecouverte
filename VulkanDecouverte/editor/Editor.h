#pragma once

#include "InspectorWindow.h"
#include "../GuiHandler.h"
#include "../RenderWindow.h"

class Mesh;
struct NodeEditor;

class Editor
{
public:
    Editor(GuiHandler* guiHandler);
    ~Editor();
    
    void draw();

private:
    // Main app window
    RenderWindow* m_renderWindow;
    GuiHandler*     m_guiHandler;
    int m_mainWindowContext;

    // Scene view
    RenderTarget* m_renderScene;

    // Window in editor
    InspectorWindow m_inspectorWindow;
    NodeEditor*     m_nodeEditor;

    // Other for tests
    RenderPipeline* m_renderPipeline;

    RenderObject* m_testObject;
    Mesh* m_mesh;

    Texture* m_defaultTexture;
    Sampler* m_defaultSampler;

    VkDescriptorSet renderedImages[RenderWindow::MAX_FRAMES_IN_FLIGHT];
    
    
};
