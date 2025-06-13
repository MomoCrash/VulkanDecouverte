#pragma once

#include "framework.h"

class Shader;
class RenderTarget;
class RenderWindow;

class RenderPipeline
{
public:
    RenderPipeline(std::vector<Shader*> shaders, RenderWindow const& window);
    RenderPipeline(std::vector<Shader*> shaders, RenderWindow const& window, RenderTarget const& target);
    ~RenderPipeline();

    VkPipeline& getGraphicsPipeline();

private:
    VkPipeline m_graphicsPipeline;
};
