#pragma once

#include "framework.h"

class Shader;
class RenderTarget;
class RenderWindow;

class RenderPipeline
{
public:
    RenderPipeline(std::vector<Shader*> shaders, RenderWindow& window);
    ~RenderPipeline();

    VkPipeline& getGraphicsPipeline();

private:
    VkPipeline m_graphicsPipeline;
};
