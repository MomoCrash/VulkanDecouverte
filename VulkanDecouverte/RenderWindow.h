#pragma once

#include <chrono>

#include "framework.h"

#include "Application.h"
#include "Window.h"
#include "RenderTarget.h"

class RenderPipeline;
class RenderObject;

class RenderWindow : public Window {

	struct UniformBufferObject {
		mat4 view;
		mat4 proj;
	} ubo;

	struct UboDataDynamic {
		mat4* model{ nullptr };
	} dynamicUbo;

public:
	const int MAX_FRAMES_IN_FLIGHT = 2;

	RenderWindow(const char* windowTitle, int width, int height);
	~RenderWindow();

	void Initialize();
	
	void createSurface();
	void createLogicalDevice();
	void createSwapChain();
	void createRenderPass();
	void createImageViews();
	void createDescriptorSetLayout();
	void createFramebuffers();
	void createCommandPool();
	void createDepthResources();
	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();
	void createCommandBuffer();
	void createSyncObjects();
	void recreateSwapchain();
	
	void createBuffer(VkBufferUsageFlags usages, VkMemoryPropertyFlags flags,
	VkBuffer& buffer, VkDeviceMemory& uploader, uint64_t size);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkExtent2D const& getExtent2D();
	VkRenderPass const& getRenderPass();
	VkDescriptorSetLayout& getDescriptorLayout();
	VkCommandBuffer const& getCommandBuffer();
	VkSurfaceKHR& getSurface();

	VkPipelineLayout& getPipelineLayout();

	void update();
	
	void clear();
	void draw(RenderPipeline& pipeline, RenderObject& object);
	void display();
	
private:
	// All of this come from an older version of the app when Application class don't exist now all of this is global context of window
	// 	VkInstance m_instance; // Vulkan Global Instance
	// 	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE; // Graphic Card Used
	//
	// 	VkDebugUtilsMessengerEXT m_debugMessenger; // Debugger
	//
	// #ifdef NDEBUG
	// 	bool m_enableValidationLayers = false;
	// #else
	// 	bool m_enableValidationLayers = true;
	// #endif
	//
	// 	bool rateDeviceSuitability(VkPhysicalDevice device);
	// 	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	// 	std::vector<const char*> getRequiredExtensions();
	//  bool checkValidationSupport();

	// Reference to the device (Replace code on top)
	
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	uint32 frameCounter;
	VkDevice const* m_device;

	// Main device element
	VkSurfaceKHR m_surface;

	// Swapchain datas
	VkSwapchainKHR m_swapchain;
	std::vector<VkImage> m_swapChainImages;
	std::vector<VkImageView> m_swapChainImageViews;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;

	// Render pipeline (PSO)
	RenderTarget* m_renderTarget;
	
	VkRenderPass m_renderPass;
	
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorPool m_descriptorPool;
	std::vector<VkDescriptorSet> m_descriptorSets;
	
	// Command list
	VkCommandPool m_commandPool;
	std::vector<VkCommandBuffer> m_commandBuffers;

	// Synchronization objects
	uint32_t currentFrame = 0;
	uint32_t m_imageIndex = 0;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;

	// Constant buffers

	size_t dynamicAlignment{ 0 };
	uint currentObject;
	
	std::vector<VkBuffer>		m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory;
	std::vector<void*>			m_uniformBuffersMapped;

	std::vector<VkBuffer>		m_dynamicUniformBuffers;
	std::vector<VkDeviceMemory> m_dynamicUniformBuffersMemory;
	std::vector<void*>			m_dynamicUniformBuffersMapped;
	

	VkClearValue m_clearColor = { {{0.0f, 0.2f, 0.0f, 1.0f}} };

	// Need this because some drivers don't call resize
	bool framebufferResized = false;
	
	VkViewport m_viewport;
	VkRect2D m_scissor;
	
	vec3 position = vec3(0.0f, 0.0f, -3.0f);

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);
	
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void cleanupSwapChain();
	uint32_t flushCommand();

};