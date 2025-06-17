#pragma once

#include "../framework.h"
#include "../RenderWindow.h"
#include "../GuiHandler.h"

struct NodeEditor : ImFlow::BaseNode
{
    ImFlow::ImNodeFlow mINF;
    RenderWindow* window;
    GuiHandler* contextGuiHandlers;

    int index = 0;
    bool m_isOpen = false;
    
    NodeEditor(GuiHandler* handler);
    ~NodeEditor();

    void open();
    void close();
    void draw();
    
};
