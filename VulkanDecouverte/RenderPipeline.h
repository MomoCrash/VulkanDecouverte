#pragma once

#include "framework.h"

class Sampler;
class Texture;
class Shader;
class RenderTarget;
class RenderWindow;

struct UniformBufferObject {
    mat4 view;
    mat4 proj;
};

struct UboDataDynamic {
    mat4* model{ nullptr };
};

class RenderPipeline
{
public:

    static const int MAX_OBJECT_RENDERER = 150;

    RenderPipeline(Texture& texture, Sampler& sampler, RenderTarget& target, std::vector<Shader*> shaders);
    ~RenderPipeline();

    VkPipeline& getGraphicsPipeline();
    VkPipelineLayout& getGraphicsPipelineLayout();
    VkDescriptorSetLayout& GetDescriptorSetLayout();
    
    VkDescriptorSet& GetDescriptorSets(uint32_t frame);
    void* GetUniformBuffer(uint32_t frame);
    void* GetDynamicBuffer(uint32_t frame);

    void Update(UniformBufferObject& buffer, uint32_t frame);
    void UpdatePerObject(UboDataDynamic* buffer, uint32_t frame);
    
private:
    
    VkPipelineCache m_pipelineCache{ VK_NULL_HANDLE };
    VkPipelineLayout m_pipelineLayout { VK_NULL_HANDLE };
    VkPipeline m_graphicsPipeline{ VK_NULL_HANDLE };

    VkDescriptorPool m_descriptorPool{ VK_NULL_HANDLE };
    VkDescriptorSetLayout m_descriptorSetLayout{ VK_NULL_HANDLE };
    std::vector<VkDescriptorSet> m_descriptorSets{ VK_NULL_HANDLE };

    std::vector<VkBuffer>		m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;
    std::vector<void*>			m_uniformBuffersMapped;

    std::vector<VkBuffer>		m_dynamicUniformBuffers;
    std::vector<VkDeviceMemory> m_dynamicUniformBuffersMemory;
    std::vector<void*>			m_dynamicUniformBuffersMapped;

    void createDescriptorPool();
    void createDescriptorSets(Texture& texture, Sampler& sampler);
    void createUniformBuffers();

};
