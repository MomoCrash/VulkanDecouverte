#pragma once

#include <chrono>

#include "framework.h"
#include "RenderPipeline.h"
#include "RenderTarget.h"

#include "Window.h"

class Texture;
class Sampler;
class RenderPipeline;
class RenderObject;

class RenderWindow : public Window {

public:
	static const int MAX_FRAMES_IN_FLIGHT = 2;

	RenderWindow(const char* windowTitle, int width, int height);
	~RenderWindow();
	
	void createSurface();
	void createSwapChain();
	void createImageViews();
	void createFramebuffers();
	void createDepthResources();
	void createSyncObjects();
	void recreateSwapchain();
	
	VkExtent2D const& getExtent2D();
	VkSurfaceKHR& getSurface();

	RenderTarget* getRenderTarget();
	RenderContext* getRenderContext();
	uint32_t getCurrentFrame();

	void update();

	void clear();
	
	void drawObject(RenderPipeline& pipeline, RenderObject& object);
	
	void display();

	bool shouldClose();

protected:
	// Reference to device
	VkDevice const* m_device;
	
	// Use for frame tracking
	std::chrono::time_point<std::chrono::high_resolution_clock> lastTime;
	uint32 frameCounter;

	// Main device element
	VkSurfaceKHR m_surface;

	RenderTarget* m_renderTarget;
	RenderContext* m_renderContext;

	UniformBufferObject m_globalBuffer;
	UboDataDynamic m_perObjectBuffer;

	
	// Swapchain datas
	VkSwapchainKHR m_swapchain;
	std::vector<VkImage> m_swapChainImages;
	std::vector<VkImageView> m_swapChainImageViews;
	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;

	// Synchronization objects
	uint32_t currentFrame = 0;
	uint32_t m_imageIndex = 0;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkFence> m_inFlightFences;

	// Constant buffers
	uint currentObject;
	
	VkClearValue m_clearColor = { {{0.0f, 0.2f, 0.0f, 1.0f}} };

	// Need this because some drivers don't call resize
	bool framebufferResized = false;
	
	VkViewport m_viewport;
	VkRect2D m_scissor;
	
	vec3 position = vec3(0.0f, 0.0f, -3.0f);
	
	bool hasStencilComponent(VkFormat format);
	
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	void cleanupSwapChain();
	uint32_t flushCommand();

};