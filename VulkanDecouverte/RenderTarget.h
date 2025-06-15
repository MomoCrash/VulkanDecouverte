#pragma once

#include "framework.h"

class RenderWindow;

class RenderTarget
{
public:

    RenderTarget();
    RenderTarget(int width, int height);
    RenderTarget(RenderWindow* window);
    ~RenderTarget();

    VkPipelineLayout& getPipelineLayout();

private:
    
    VkPipelineLayout m_pipelineLayout;
    
};
