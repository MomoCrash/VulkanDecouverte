#include "RenderTarget.h"

#include "Application.h"
#include "Mesh.h"
#include "RenderWindow.h"

RenderTarget::RenderTarget(): m_pipelineLayout(nullptr) {}

RenderTarget::RenderTarget(int width, int height)
{

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    // USE ON A WINDOW //pipelineLayoutInfo.pSetLayouts = &window.getDescriptorLayout(); // Optional
   //pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    if (vkCreatePipelineLayout(Application::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    
}

RenderTarget::RenderTarget(RenderWindow* window)
{
    
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1; // Optional
    pipelineLayoutInfo.pSetLayouts = &window->getDescriptorLayout(); // Optional
    pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
    pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional

    VkResult result = vkCreatePipelineLayout(Application::getInstance()->getDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    
}

RenderTarget::~RenderTarget()
{
    vkDestroyPipelineLayout(Application::getInstance()->getDevice(), m_pipelineLayout, nullptr);
}

VkPipelineLayout const& RenderTarget::getPipelineLayout() const
{
    return m_pipelineLayout;
}

