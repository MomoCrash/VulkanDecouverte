#include "RenderPipeline.h"

#include <vector>

#include "Application.h"
#include "Mesh.h"
#include "RenderWindow.h"
#include "Sampler.h"
#include "Shader.h"
#include "Texture.h"

RenderPipeline::RenderPipeline(Texture& texture, Sampler& sampler, RenderTarget& target, std::vector<Shader*> shaders)
{

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        {0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr},
        {1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr},
        {2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr},
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    layoutInfo.pBindings = setLayoutBindings.data();

    if (vkCreateDescriptorSetLayout(Application::getInstance()->getDevice(), &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VkResult result = vkCreatePipelineLayout(Application::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    std::vector<VkPipelineShaderStageCreateInfo> infos;

    for (auto& shader : shaders)
    {
        infos.push_back(shader->getShaderInformation());
    }

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;
    
    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    /*VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) window.getExtent2D().width;
    viewport.height = (float) window.getExtent2D().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = window.getExtent2D();*/

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f; // Optional
    rasterizer.depthBiasClamp = 0.0f; // Optional
    rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f; // Optional
    multisampling.pSampleMask = nullptr; // Optional
    multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
    multisampling.alphaToOneEnable = VK_FALSE; // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f; // Optional
    colorBlending.blendConstants[1] = 0.0f; // Optional
    colorBlending.blendConstants[2] = 0.0f; // Optional
    colorBlending.blendConstants[3] = 0.0f; // Optional

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo {};
    pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pipelineDepthStencilStateCreateInfo.depthTestEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = VK_TRUE;
    pipelineDepthStencilStateCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    vkCreatePipelineCache(Application::getInstance()->getDevice(), &pipelineCacheCreateInfo, nullptr, &m_pipelineCache);
    
    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = static_cast<uint32_t>(infos.size());
    pipelineInfo.pStages = infos.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &pipelineDepthStencilStateCreateInfo;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = target.getRenderPass();
    pipelineInfo.flags = 0;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineIndex = -1;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(Application::getInstance()->getDevice(), m_pipelineCache, 1,
                                  &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    createDescriptorPool();
    createUniformBuffers();
    createDescriptorSets(texture, sampler);
    
}

RenderPipeline::~RenderPipeline()
{
    
    vkDestroyDescriptorSetLayout(Application::getInstance()->getDevice(), m_descriptorSetLayout, nullptr);
    
    vkDestroyPipelineLayout(Application::getInstance()->getDevice(), m_pipelineLayout, nullptr);

    vkDestroyPipelineCache(Application::getInstance()->getDevice(), m_pipelineCache, nullptr);
    
    vkDestroyPipeline(Application::getInstance()->getDevice(), m_graphicsPipeline, nullptr);

    vkDestroyDescriptorPool(Application::getInstance()->getDevice(), m_descriptorPool, nullptr);

    for (size_t i = 0; i < RenderWindow::MAX_FRAMES_IN_FLIGHT; i++) {

        // Buffers
        vkDestroyBuffer(Application::getInstance()->getDevice(), m_uniformBuffers[i], nullptr);
        vkFreeMemory(Application::getInstance()->getDevice(), m_uniformBuffersMemory[i], nullptr);

        // Buffers
        vkDestroyBuffer(Application::getInstance()->getDevice(), m_dynamicUniformBuffers[i], nullptr);
        vkFreeMemory(Application::getInstance()->getDevice(), m_dynamicUniformBuffersMemory[i], nullptr);
        
    }
    
}

void RenderPipeline::createDescriptorPool()
{

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(RenderWindow::MAX_FRAMES_IN_FLIGHT) },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, static_cast<uint32_t>(RenderWindow::MAX_FRAMES_IN_FLIGHT) },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(RenderWindow::MAX_FRAMES_IN_FLIGHT) },
    };
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(RenderWindow::MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(Application::getInstance()->getDevice(), &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void RenderPipeline::createDescriptorSets(Texture& texture, Sampler& sampler)
{
    std::vector<VkDescriptorSetLayout> layouts(RenderWindow::MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.pSetLayouts = layouts.data();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());

    m_descriptorSets.resize(RenderWindow::MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(Application::getInstance()->getDevice(),
        &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < RenderWindow::MAX_FRAMES_IN_FLIGHT; i++) {

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorBufferInfo dBufferInfo{};
        dBufferInfo.buffer = m_dynamicUniformBuffers[i];
        dBufferInfo.offset = 0;
        dBufferInfo.range = Application::getDynamicAlignment();

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture.getImageView();
        imageInfo.sampler   = sampler.getSampler();

        VkWriteDescriptorSet writeDescriptorSet {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = m_descriptorSets[i];
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pBufferInfo = &bufferInfo;
        writeDescriptorSet.descriptorCount = 1;
        
        VkWriteDescriptorSet writeDynamicDescriptorSet {};
        writeDynamicDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDynamicDescriptorSet.dstSet = m_descriptorSets[i];
        writeDynamicDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        writeDynamicDescriptorSet.dstBinding = 1;
        writeDynamicDescriptorSet.pBufferInfo = &dBufferInfo;
        writeDynamicDescriptorSet.descriptorCount = 1;

        VkWriteDescriptorSet writeTextureDescriptorSet {};
        writeTextureDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeTextureDescriptorSet.dstSet = m_descriptorSets[i];
        writeTextureDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeTextureDescriptorSet.dstBinding = 2;
        writeTextureDescriptorSet.descriptorCount = 1;
        writeTextureDescriptorSet.pImageInfo = &imageInfo;

        std::vector<VkWriteDescriptorSet> writeDescriptorSets = {
            writeDescriptorSet,
            writeDynamicDescriptorSet,
            writeTextureDescriptorSet
        };

        vkUpdateDescriptorSets(Application::getInstance()->getDevice(), static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        
    }
    
}

void RenderPipeline::createUniformBuffers()
{
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(RenderWindow::MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMemory.resize(RenderWindow::MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped.resize(RenderWindow::MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < RenderWindow::MAX_FRAMES_IN_FLIGHT; i++) {
        Application::getInstance()->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_uniformBuffers[i], m_uniformBuffersMemory[i], bufferSize);

        vkMapMemory(Application::getInstance()->getDevice(), m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
    }

    std::cout << "minUniformBufferOffsetAlignment = " << Application::getDynamicAlignment() << std::endl;
    std::cout << "dynamicAlignment = " << Application::getDynamicAlignment() << std::endl;

    m_dynamicUniformBuffers.resize(RenderWindow::MAX_FRAMES_IN_FLIGHT);
    m_dynamicUniformBuffersMemory.resize(RenderWindow::MAX_FRAMES_IN_FLIGHT);
    m_dynamicUniformBuffersMapped.resize(RenderWindow::MAX_FRAMES_IN_FLIGHT);

    size_t dBufferSize = MAX_OBJECT_RENDERER * Application::getDynamicAlignment();
    for (size_t i = 0; i < RenderWindow::MAX_FRAMES_IN_FLIGHT; i++) {
        Application::getInstance()->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            m_dynamicUniformBuffers[i], m_dynamicUniformBuffersMemory[i], dBufferSize);

        vkMapMemory(Application::getInstance()->getDevice(), m_dynamicUniformBuffersMemory[i], 0, dBufferSize, 0, &m_dynamicUniformBuffersMapped[i]);
    }
}


VkPipeline& RenderPipeline::getGraphicsPipeline()
{
    return m_graphicsPipeline;
}

VkPipelineLayout& RenderPipeline::getGraphicsPipelineLayout()
{
    return m_pipelineLayout;
}

VkDescriptorSetLayout& RenderPipeline::GetDescriptorSetLayout()
{
    return m_descriptorSetLayout;
}

VkDescriptorSet& RenderPipeline::GetDescriptorSets(uint32_t frame)
{
    return m_descriptorSets[frame];
}

void* RenderPipeline::GetUniformBuffer(uint32_t frame)
{
    return m_uniformBuffersMapped[frame];
}

void* RenderPipeline::GetDynamicBuffer(uint32_t frame)
{
    return m_dynamicUniformBuffersMapped[frame];
}

void RenderPipeline::Update(UniformBufferObject& buffer, uint32_t frame)
{
    
    memcpy(m_uniformBuffersMapped[frame], &buffer, sizeof(buffer));
    
}

void RenderPipeline::UpdatePerObject(UboDataDynamic* buffer, uint32_t frame)
{
    memcpy(m_dynamicUniformBuffersMapped[frame], buffer->model, MAX_OBJECT_RENDERER * Application::getDynamicAlignment());
}
