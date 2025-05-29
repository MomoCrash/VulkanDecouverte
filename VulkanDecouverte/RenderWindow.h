#pragma once

#include "framework.h"

#include "Shader.h"
#include "Window.h"

class RenderObject;

struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct UniformBufferObject {
	mat4 model;
	mat4 view;
	mat4 proj;
};

class RenderWindow : public Window {

	const int MAX_FRAMES_IN_FLIGHT = 2;

public:
	RenderWindow(const char* title, const int width, const int height);
	~RenderWindow();

	void setupImgui();

	void setupVulkan();
	void setupDebugMessenger();
	void createSurface();
	void pickPhysicalDevice();
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
	
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	std::vector<const char*> getRequiredExtensions();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		
	const VkDevice& getDevice();
	const VkExtent2D& getExtent2D();
	const VkRenderPass& getRenderPass();
	const VkDescriptorSetLayout& getDescriptorLayout();
	const VkCommandBuffer& getCommandBuffer();

	void update();
	
	void clear();
	void draw(RenderObject& buffer, Shader& shader, mat4& transform);
	void display();
	
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
private:
	VkInstance m_instance; // Vulkan Global Instance
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE; // Graphic Card Used
	VkDebugUtilsMessengerEXT m_debugMessenger; // Debugger

	// Main device elements
	VkDevice m_device;
	VkQueue m_presentQueue;
	VkQueue m_graphicsQueue;
	VkSurfaceKHR m_surface;

	// Swapchain datas
	VkSwapchainKHR m_swapchain;
	std::vector<VkImage> m_swapChainImages;
	std::vector<VkImageView> m_swapChainImageViews;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;

	// Render pipeline (PSO)
	VkRenderPass m_renderPass;
	
	VkDescriptorSetLayout m_descriptorSetLayout;
	VkDescriptorPool m_descriptorPool;
	std::vector<VkDescriptorSet> m_descriptorSets;

	VkDescriptorPool m_imguiPool;
	
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
	std::vector<VkBuffer> m_uniformBuffers;
	std::vector<VkDeviceMemory> m_uniformBuffersMemory;
	std::vector<void*> m_uniformBuffersMapped;

	VkClearValue m_clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

	// Need this because some drivers don't call resize
	bool framebufferResized = false;
	
	const std::vector<const char*> m_deviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::vector<const char*> m_validationLayers = {
		"VK_LAYER_KHRONOS_validation"
	};

	#ifdef NDEBUG
		bool m_enableValidationLayers = false;
	#else
		bool m_enableValidationLayers = true;
	#endif

	VkViewport m_viewport;
	VkRect2D m_scissor;

	UniformBufferObject ubo{};

	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);
	
	bool rateDeviceSuitability(VkPhysicalDevice device);
	bool checkValidationSupport();
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void cleanupSwapChain();
	uint32_t flushCommand();

};