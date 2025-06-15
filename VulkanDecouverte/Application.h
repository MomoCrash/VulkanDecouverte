#pragma once

#include "framework.h"

class RenderWindow;

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

class Application
{
public:
    ~Application();
    void Initialize(const char* appName);

    VkSurfaceKHR* createSurface(RenderWindow& window);
    void setupDebugLayer();
    void setupPhysicalDevice(VkSurfaceKHR& surface);
    void setupLogicalDevice(VkSurfaceKHR& surface);
    
    static Application* getInstance();
    
    VkInstance& getVulkanInstance();
    VkDevice const& getDevice();
    VkPhysicalDevice const& getPhysicalDevice();

    bool const& useValidationLayer();
    std::vector<const char*> const& getDeviceExtensions();
    std::vector<const char*> const& getValidationLayer();

    VkQueue const& getGraphicQueue();
    VkQueue const& getPresentQueue();

    VkPhysicalDeviceProperties& GetPhysicalDeviceProperties();
    VkPhysicalDeviceFeatures& GetPhysicalDeviceFeatures();

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);

private:
    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

    int rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR& surface);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    bool checkValidationSupport();
    
    std::vector<const char*> getRequiredExtensions() const;

    VkInstance m_instance = nullptr; // Vulkan Global Instance
    
    VkDevice m_device = nullptr;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE; // Graphic Card Used
    VkQueue m_presentQueue = nullptr;
    VkQueue m_graphicsQueue = nullptr;

    VkPhysicalDeviceProperties m_deviceProperties;
    VkPhysicalDeviceFeatures m_deviceFeatures;

    VkDebugUtilsMessengerEXT m_debugMessenger = nullptr; // Debugger

#ifdef NDEBUG
    bool m_enableValidationLayers = false;
#else
    bool m_enableValidationLayers = true;
#endif

    std::vector<const char*> m_deviceExtensions = {
    	VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    std::vector<const char*> m_validationLayers = {
    	"VK_LAYER_KHRONOS_validation"
    };

};
