#pragma once

#include "framework.h"

class RenderWindow;

class UIHandler
{
public:
    UIHandler();
    ~UIHandler();

    void inject(RenderWindow* window);

private:
    VkDescriptorPool m_imguiPool;

};
