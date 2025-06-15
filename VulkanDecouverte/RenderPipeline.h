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
    VkPipelineCache m_pipelineCache{ VK_NULL_HANDLE };
    VkPipeline m_graphicsPipeline{ VK_NULL_HANDLE };
};
