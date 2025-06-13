#include "Shader.h"

#include <fstream>

#include "Mesh.h"
#include "RenderWindow.h"

Shader::Shader(std::string shaderPath, Type shaderType)
{
    
    std::vector<char> shaderCode = readFile(SHADER_FOLDER + shaderPath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = shaderCode.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());
    
    if (vkCreateShaderModule(Application::getInstance()->getDevice(), &createInfo, nullptr, &mShaderModule) != VK_SUCCESS) {
        throw std::runtime_error("failed to create shader module!");
    }
    
    mPipelineShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    mPipelineShaderStageInfo.stage = static_cast<VkShaderStageFlagBits>(shaderType);
    mPipelineShaderStageInfo.module = mShaderModule;
    mPipelineShaderStageInfo.pName = "main";
    
    
}

Shader::~Shader()
{
    
    vkDestroyShaderModule(Application::getInstance()->getDevice(), mShaderModule, nullptr);
    
}

std::vector<char> Shader::readFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open " + filename );
    }

    size_t fileSize = (size_t) file.tellg();
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);

    file.close();

    return buffer;
}

VkPipelineShaderStageCreateInfo const& Shader::getShaderInformation()
{
    return mPipelineShaderStageInfo;
}
