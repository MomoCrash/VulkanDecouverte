#include "RenderWindow.h"

#include <algorithm>
#include <chrono>

#include "Mesh.h"
#include "RenderObject.h"
#include "RenderPipeline.h"
#include "Sampler.h"
#include "Texture.h"

void* alignedAlloc(size_t size, size_t alignment)
{
    void *data = nullptr;
#if defined(_MSC_VER) || defined(__MINGW32__)
    data = _aligned_malloc(size, alignment);
#else
    int res = posix_memalign(&data, alignment, size);
    if (res != 0)
        data = nullptr;
#endif
    return data;
}

RenderWindow::RenderWindow(const char* name, const int width, const int height)
    : Window(name, width, height), m_device(&Application::getInstance()->getDevice())
{

    ubo.proj = mat4(1.0f);
    ubo.view = mat4(1.0f);

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, [](GLFWwindow* window, int width, int height)
    {
        auto app = reinterpret_cast<RenderWindow*>(glfwGetWindowUserPointer(window));
        app->framebufferResized = true;
    });

    createSurface();

    Application::getInstance()->setupPhysicalDevice(getSurface());
    Application::getInstance()->setupLogicalDevice(getSurface());

    Initialize();
    
}

RenderWindow::~RenderWindow()
{

    delete m_renderTarget;

    cleanupSwapChain();

    delete m_defaultSampler;
    delete m_defaultTexture;
    
    vkDestroyRenderPass(*m_device, m_renderPass, nullptr);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        vkDestroySemaphore(*m_device, m_renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(*m_device, m_imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(*m_device, m_inFlightFences[i], nullptr);

        // Buffers
        vkDestroyBuffer(*m_device, m_uniformBuffers[i], nullptr);
        vkFreeMemory(*m_device, m_uniformBuffersMemory[i], nullptr);

        // Buffers
        vkDestroyBuffer(*m_device, m_dynamicUniformBuffers[i], nullptr);
        vkFreeMemory(*m_device, m_dynamicUniformBuffersMemory[i], nullptr);
    }
    
    vkDestroyDescriptorSetLayout(*m_device, m_descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(*m_device, m_descriptorPool, nullptr);

    vkDestroyCommandPool(*m_device, m_commandPool, nullptr);

    vkDestroySurfaceKHR(Application::getInstance()->getVulkanInstance(), m_surface, nullptr);
}

void RenderWindow::Initialize()
{
    
    createSwapChain();
    
    createRenderPass();

    createImageViews();
    
    createDescriptorSetLayout();

    m_renderTarget = new RenderTarget(this);

    createFramebuffers();

    createCommandPool();

    createDepthResources();
    
    createUniformBuffers();

    createDescriptorPool();

    m_defaultTexture = new Texture(*this, "sunflower.jpg");
    m_defaultSampler = new Sampler();
    
    createDescriptorSets();
    
    createCommandBuffer();

    createSyncObjects();
    
    
}

void RenderWindow::createSurface()
{
    m_surface = *Application::getInstance()->createSurface(*this);
}

void RenderWindow::createSwapChain()
{
    SwapChainSupportDetails swapChainSupport = Application::getInstance()->querySwapChainSupport(Application::getInstance()->getPhysicalDevice(), m_surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
    VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
    VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

    m_swapChainImageFormat = surfaceFormat.format;
    m_swapChainExtent = extent;

    uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
    if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
        imageCount = swapChainSupport.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = m_surface;

    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    QueueFamilyIndices indices = Application::getInstance()->findQueueFamilies(Application::getInstance()->getPhysicalDevice(), m_surface);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0; // Optional
        createInfo.pQueueFamilyIndices = nullptr; // Optional
    }

    createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;
    
    VkResult result = vkCreateSwapchainKHR(*m_device, &createInfo, nullptr, &m_swapchain);
    if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to create swap chain!");
    }
    
    vkGetSwapchainImagesKHR(*m_device, m_swapchain, &imageCount, nullptr);
    m_swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(*m_device, m_swapchain, &imageCount, m_swapChainImages.data());
}

void RenderWindow::createRenderPass()
{
    
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = m_swapChainImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    if (vkCreateRenderPass(*m_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS) {
        throw std::runtime_error("failed to create render pass!");
    }
}

void RenderWindow::createImageViews()
{
    m_swapChainImageViews.resize(m_swapChainImages.size());

    for (size_t i = 0; i < m_swapChainImages.size(); i++) {

        m_swapChainImageViews[i] = createImageView(m_swapChainImages[i], m_swapChainImageFormat);
    }
}

void RenderWindow::createDescriptorSetLayout()
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

    if (vkCreateDescriptorSetLayout(*m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
}

void RenderWindow::createFramebuffers()
{
    m_swapChainFramebuffers.resize(m_swapChainImageViews.size());

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
        VkImageView attachments[] = {
            m_swapChainImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = m_swapChainExtent.width;
        framebufferInfo.height = m_swapChainExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(*m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i]) != VK_SUCCESS) {
            throw std::runtime_error("failed to create framebuffer!");
        }
    }
}

void RenderWindow::createCommandPool()
{
    
    QueueFamilyIndices queueFamilyIndices = Application::getInstance()->findQueueFamilies(Application::getInstance()->getPhysicalDevice(), m_surface);

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    if (vkCreateCommandPool(*m_device, &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
    
}

void RenderWindow::createDepthResources()
{
    
    VkFormat depthFormat = findDepthFormat();
    
}

void RenderWindow::createUniformBuffers()
{
    
    VkDeviceSize bufferSize = sizeof(UniformBufferObject);

    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        Application::getInstance()->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            m_uniformBuffers[i], m_uniformBuffersMemory[i], bufferSize);

        vkMapMemory(*m_device, m_uniformBuffersMemory[i], 0, bufferSize, 0, &m_uniformBuffersMapped[i]);
    }

    size_t minUboAlignment = Application::getInstance()->GetPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
    dynamicAlignment = sizeof(glm::mat4);
    if (minUboAlignment > 0) {
        dynamicAlignment = (dynamicAlignment + minUboAlignment - 1) & ~(minUboAlignment - 1);
    }

    size_t dBufferSize = 125 * dynamicAlignment;
    dynamicUbo.model = (glm::mat4*)alignedAlloc(dBufferSize, dynamicAlignment);
    assert(dynamicUbo.model);

    std::cout << "minUniformBufferOffsetAlignment = " << minUboAlignment << std::endl;
    std::cout << "dynamicAlignment = " << dynamicAlignment << std::endl;

    m_dynamicUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_dynamicUniformBuffersMemory.resize(MAX_FRAMES_IN_FLIGHT);
    m_dynamicUniformBuffersMapped.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        Application::getInstance()->createBuffer(VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            m_dynamicUniformBuffers[i], m_dynamicUniformBuffersMemory[i], dBufferSize);

        vkMapMemory(*m_device, m_dynamicUniformBuffersMemory[i], 0, dBufferSize, 0, &m_dynamicUniformBuffersMapped[i]);
    }
    
}

void RenderWindow::createDescriptorPool()
{

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT) },
    };
    
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    if (vkCreateDescriptorPool(*m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

void RenderWindow::createDescriptorSets()
{
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout);
    
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.pSetLayouts = layouts.data();
    allocInfo.descriptorSetCount = static_cast<uint32_t>(layouts.size());

    m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    if (vkAllocateDescriptorSets(*m_device, &allocInfo, m_descriptorSets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {

        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkDescriptorBufferInfo dBufferInfo{};
        dBufferInfo.buffer = m_dynamicUniformBuffers[i];
        dBufferInfo.offset = 0;
        dBufferInfo.range = dynamicAlignment;

        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = m_defaultTexture->getImageView();
        imageInfo.sampler = m_defaultSampler->getSampler();

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

        vkUpdateDescriptorSets(*m_device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
        
    }
    
}

void RenderWindow::createCommandBuffer()
{

    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = (uint32_t) m_commandBuffers.size();

    if (vkAllocateCommandBuffers(*m_device, &allocInfo, m_commandBuffers.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    
}

void RenderWindow::createSyncObjects()
{
    
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (vkCreateSemaphore(*m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(*m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(*m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
    }
    
}

void RenderWindow::recreateSwapchain()
{
    // TODO Remove that because pause app when minimized
    int width = 0, height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    while (width == 0 || height == 0) {
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }
    
    vkDeviceWaitIdle(*m_device);

    cleanupSwapChain();

    createSwapChain();
    createImageViews();
    createFramebuffers();
}

VkImageView RenderWindow::createImageView(VkImage image, VkFormat format)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    if (vkCreateImageView(Application::getInstance()->getDevice(), &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture image view!");
    }

    return imageView;
}

VkSurfaceFormatKHR RenderWindow::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats) {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            return availableFormat;
        }
    }

    return availableFormats[0];
}

VkPresentModeKHR RenderWindow::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes) {
        // We ue mailing method because remove tearing and render fastest as possible
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D RenderWindow::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
        return capabilities.currentExtent;
    } else {
        int width, height;
        glfwGetFramebufferSize(m_window, &width, &height);

        VkExtent2D actualExtent = {
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height)
        };

        actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        return actualExtent;
    }
}

void RenderWindow::cleanupSwapChain()
{
    for (size_t i = 0; i < m_swapChainFramebuffers.size(); i++) {
        vkDestroyFramebuffer(*m_device, m_swapChainFramebuffers[i], nullptr);
    }

    for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
        vkDestroyImageView(*m_device, m_swapChainImageViews[i], nullptr);
    }

    vkDestroySwapchainKHR(*m_device, m_swapchain, nullptr);
}

uint32_t RenderWindow::flushCommand()
{
    /*
     * Wait for the previous frame to finish
     * Acquire an image from the swap chain
     * Record a command buffer which draws the scene onto that image
     * Submit the recorded command buffer
     * Present the swap chain image
    */

    // Refer to https://vulkan-tutorial.com/en/Drawing_a_triangle/Drawing/Rendering_and_presentation
    // Semaphores for sync the GPU with signal
    // And Fences for sync the CPU with GPU
    
    vkWaitForFences(*m_device, 1, &m_inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(*m_device, m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        framebufferResized = false;
        recreateSwapchain();
        return imageIndex;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("failed to acquire swap chain image!");
    }

    vkResetFences(*m_device, 1, &m_inFlightFences[currentFrame]);
    
    vkResetCommandBuffer(m_commandBuffers[currentFrame], 0);
    return imageIndex;
}

VkCommandBuffer RenderWindow::beginSingleTimeCommands()
{

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(*m_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void RenderWindow::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
    
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(Application::getInstance()->getGraphicQueue(), 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(Application::getInstance()->getGraphicQueue());

    vkFreeCommandBuffers(*m_device, m_commandPool, 1, &commandBuffer);
    
}

void RenderWindow::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{

    VkCommandBuffer commandBuffer = beginSingleTimeCommands();

    VkBufferCopy copyRegion{};
    copyRegion.srcOffset = 0; // Optional
    copyRegion.dstOffset = 0; // Optional
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    endSingleTimeCommands(commandBuffer);
    
}

const VkExtent2D& RenderWindow::getExtent2D()
{
    return m_swapChainExtent;
}

const VkRenderPass& RenderWindow::getRenderPass()
{
    return m_renderPass;
}


VkDescriptorSetLayout& RenderWindow::getDescriptorLayout()
{
    return m_descriptorSetLayout;
}

const VkCommandBuffer& RenderWindow::getCommandBuffer()
{
    return m_commandBuffers[currentFrame];
}

VkSurfaceKHR& RenderWindow::getSurface()
{
    return m_surface;
}

VkPipelineLayout& RenderWindow::getPipelineLayout()
{
    return m_renderTarget->getPipelineLayout();
}

void RenderWindow::update()
{

    // Update Uniform Bffer
    auto now = std::chrono::high_resolution_clock::now();
    
    frameCounter++;

    ubo.view = lookAt(vec3(-5.0f, 3.0f,  -5.0f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    ubo.proj = perspective(radians(70.0f), (float)m_swapChainExtent.width / (float)m_swapChainExtent.height, 0.1f, 256.0f);
    ubo.proj[1][1] *= -1.0f;
    
    memcpy(m_uniformBuffersMapped[currentFrame], &ubo, sizeof(ubo));

    float fpsTimer = (float)(std::chrono::duration<double, std::milli>(now - lastTime).count());

    if (fpsTimer > 1000.0f)
    {
        
        uint32_t fps = static_cast<uint32_t>((float)frameCounter * (1000.0f / fpsTimer));
        
        string name = "FPS : " + std::to_string(fps);
        glfwSetWindowTitle(m_window, name.c_str());
        
        frameCounter = 0;
        lastTime = now;
        
    }

}

void RenderWindow::clear()
{

    m_imageIndex = flushCommand();
    VkCommandBuffer& buffer = m_commandBuffers[currentFrame];
    
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0; // Optional
    beginInfo.pInheritanceInfo = nullptr; // Optional

    // Flags can be
    // VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT: This is a secondary command buffer that will be entirely within a single render pass.
    // VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT: The command buffer will be rerecorded right after executing it once.
    
    if (vkBeginCommandBuffer(buffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin recording command buffer!");
    }

    // Starting a render pass
    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass;
    renderPassInfo.framebuffer = m_swapChainFramebuffers[m_imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &m_clearColor;

    vkCmdBeginRenderPass(buffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    
    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapChainExtent.width);
    viewport.height = static_cast<float>(m_swapChainExtent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(buffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChainExtent;
    vkCmdSetScissor(buffer, 0, 1, &scissor);

    currentObject = 0;
}


void RenderWindow::drawObject(RenderPipeline& pipeline, RenderObject& object)
{
    glm::mat4* modelMat = (mat4*)(((uint64_t)dynamicUbo.model + (currentObject * dynamicAlignment)));
    *modelMat = object.getTransform();
    
    memcpy(m_dynamicUniformBuffersMapped[currentFrame], dynamicUbo.model, 125 * dynamicAlignment);
    
    VkCommandBuffer& commandBuffer = m_commandBuffers[currentFrame];
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.getGraphicsPipeline());

    VkBuffer vertexBuffers[] = {
        object.getMesh()->getVertexBuffer()
    };
    
    VkDeviceSize offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
    vkCmdBindIndexBuffer(commandBuffer, object.getMesh()->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

    uint32_t dynamicOffset = currentObject * static_cast<uint32_t>(dynamicAlignment);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_renderTarget->getPipelineLayout(),
        0, 1, &m_descriptorSets[currentFrame], 1, &dynamicOffset);
    vkCmdDrawIndexed(commandBuffer, object.getMesh()->getIndexCount(), 1, 0, 0, 0);

    currentObject++;

}

void RenderWindow::draw() { }

void RenderWindow::display()
{
    
    VkCommandBuffer& buffer = m_commandBuffers[currentFrame];
    vkCmdEndRenderPass(buffer);

    if (vkEndCommandBuffer(buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to record command buffer!");
    }
    
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {m_imageAvailableSemaphores[currentFrame]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &m_commandBuffers[currentFrame];

    VkSemaphore signalSemaphores[] = {m_renderFinishedSemaphores[currentFrame]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(Application::getInstance()->getGraphicQueue(), 1, &submitInfo, m_inFlightFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit draw command buffer!");
    }
    
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {m_swapchain};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_imageIndex;
    presentInfo.pResults = nullptr; // Optional

    VkResult result = vkQueuePresentKHR(Application::getInstance()->getGraphicQueue(), &presentInfo);
    
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
        framebufferResized = false;
        recreateSwapchain();
    } else if (result != VK_SUCCESS) {
        throw std::runtime_error("failed to present swap chain image!");
    }
    
    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

VkFormat RenderWindow::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling,
    VkFormatFeatureFlags features)
{
    for (VkFormat format : candidates) {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(Application::getInstance()->getPhysicalDevice(), format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
            return format;
        } else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
            return format;
        }
    }

    throw std::runtime_error("failed to find supported format!");
    
}

VkFormat RenderWindow::findDepthFormat()
{
    return findSupportedFormat(
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

bool RenderWindow::hasStencilComponent(VkFormat format)
{
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

bool RenderWindow::shouldClose()
{
    return glfwWindowShouldClose(m_window);
}
