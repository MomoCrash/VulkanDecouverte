#pragma once

#include "framework.h"

class RenderWindow;

class Shader
{
    
public:

    typedef enum Type 
    {
        VERTEX = 0x00000001,
        TESSELLATION_CONTROL = 0x00000002,
        TESSELLATION_EVALUATION = 0x00000004,
        GEOMETRY = 0x00000008,
        FRAGMENT = 0x00000010,
        COMPUTE = 0x00000020,
    } Type;
    
    Shader(std::string shaderPath, Type shaderType);
    ~Shader();
    
    std::vector<char> readFile(const std::string& filename);
    VkPipelineShaderStageCreateInfo const& getShaderInformation();

    static const inline char* SHADER_FOLDER = "res\\shaders\\";

private:
    
    VkShaderModule mShaderModule;
    VkPipelineShaderStageCreateInfo mPipelineShaderStageInfo;
    
};
