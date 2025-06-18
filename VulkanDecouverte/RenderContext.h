#pragma once

#include "framework.h"

class RenderContext
{
public:
    RenderContext(VkSurfaceKHR& surface);
    ~RenderContext();

    void createCommandBuffer();
    void createCommandPool(VkSurfaceKHR& surface);
    
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);
    
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

    VkCommandBuffer& getCommandBuffer(uint32_t frame);

private:
    
    // Command list
    VkCommandPool m_commandPool;
    std::vector<VkCommandBuffer> m_commandBuffers;
};
