#pragma once

#include "../GuiHandler.h"
#include "../RenderWindow.h"

struct NodeEditor;

class Editor final : public RenderWindow
{
public:
    Editor(GuiHandler* guiHandler);
    ~Editor() override;
    
    void draw() override;

private:
    RenderPipeline* m_renderPipeline;
    NodeEditor*     m_nodeEditor;

    GuiHandler*     m_guiHandler;
    int m_mainWindowContext;
};
