#pragma once

#include "framework.h"

class RenderWindow;

class GuiHandler
{
public:
    GuiHandler();
    ~GuiHandler();

    int inject(RenderWindow* window);
    void remove(int index);

    void setContext(int index);

private:
    std::vector<VkDescriptorPool> m_imguiPools;
    std::vector<ImGuiContext*> m_imguiContexts;

};
