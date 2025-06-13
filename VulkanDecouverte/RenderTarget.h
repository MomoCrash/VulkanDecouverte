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

    VkPipelineLayout const& getPipelineLayout() const;

private:
    
    VkPipelineLayout m_pipelineLayout;
    
};
