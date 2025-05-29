#pragma once

#include "framework.h"

class RenderWindow;

class Shader
{
    RenderWindow* m_window;
    
    VkPipelineLayout m_pipelineLayout;
    VkPipeline m_graphicsPipeline;
public:
    Shader(RenderWindow& window, std::string vshaderPath, std::string fshaderPath);
    ~Shader();
    std::vector<char> readFile(const std::string& filename);
    VkShaderModule createShaderModule(const std::vector<char>& code);

    VkPipeline& getPipeline();
    VkPipelineLayout& getPipelineLayout();
};
