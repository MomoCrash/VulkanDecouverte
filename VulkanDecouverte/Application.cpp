#include "Application.h"

#include <map>
#include <set>

#include "RenderWindow.h"

Application::~Application()
{

    vkDestroyDevice(m_device, nullptr);

    if (Application::getInstance()->useValidationLayer()) {
        DestroyDebugUtilsMessengerEXT(Application::getInstance()->getVulkanInstance(), m_debugMessenger, nullptr);
    }
    
    vkDestroyInstance(m_instance, nullptr);

    glfwTerminate();
}

void Application::Initialize(const char* appName)
{

    static bool isInitialised = false;
    
    if (isInitialised) return;
    isInitialised = true;

    glfwInit();
    
    // MAY NOT BE IN THIS CLASS BUT IN A RENDER CONTEXT BECAUSE OF m_instance UNIQUE
    // Drivers informations
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = appName;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Engine_Name_Placeholder";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Get Render current platform
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;

    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    if (m_enableValidationLayers) {
        auto extensions = getRequiredExtensions();
        createInfo.enabledLayerCount = static_cast<uint32_t>(m_validationLayers.size());
        createInfo.ppEnabledLayerNames = m_validationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    auto extensions = getRequiredExtensions();
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

    // Create Vulkan instance
    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_instance);

    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }
    // Check for validations layers (Used for debug because Vulkan don't support them)
    if (m_enableValidationLayers && !checkValidationSupport()) {
        throw std::runtime_error("Validations layer enabled but not avaiable");
    }

    setupDebugLayer();

}

VkSurfaceKHR* Application::createSurface(RenderWindow& window)
{
    VkSurfaceKHR* surface = new VkSurfaceKHR();
    if (glfwCreateWindowSurface(m_instance, window.GetWindow(), nullptr, surface) != VK_SUCCESS) {
        const char* name = "sfd";
        std::cout << glfwGetError(&name) << std::endl;
        throw std::runtime_error("failed to create window surface!");
    }
    return surface;
}

void Application::setupDebugLayer()
{
    if (!m_enableValidationLayers) return;

    // Debug layer
    VkDebugUtilsMessengerCreateInfoEXT debugInfo{};
    debugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugInfo.pfnUserCallback = debugCallback;
    debugInfo.pUserData = this; // Optional

    if (CreateDebugUtilsMessengerEXT(m_instance, &debugInfo, nullptr, &m_debugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("failed to set up debug messenger!");
    }
}

void Application::setupPhysicalDevice(VkSurfaceKHR& surface)
{
    if (m_physicalDevice) return;
    
    // Used to pick a GPU in the computer
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);
    if (deviceCount == 0) {
        throw std::runtime_error("failed to find GPUs with Vulkan support!");
    }
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    std::multimap<int, VkPhysicalDevice> candidates;

    for (const auto& device : devices) {
        int score = rateDeviceSuitability(device, surface);
        candidates.insert(std::make_pair(score, device));
    }

    // Check if the best candidate is suitable at all
    if (candidates.rbegin()->first > 0) {
        m_physicalDevice = candidates.rbegin()->second;
    } else {
        throw std::runtime_error("failed to find a suitable GPU!");
    }
}

void Application::setupLogicalDevice(VkSurfaceKHR& surface)
{

    if (m_device) return;
    
    QueueFamilyIndices indices = findQueueFamilies(getPhysicalDevice(), surface);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

    float queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();

    createInfo.pEnabledFeatures = &deviceFeatures;

    createInfo.enabledExtensionCount = static_cast<uint32_t>(getDeviceExtensions().size());
    createInfo.ppEnabledExtensionNames = getDeviceExtensions().data();

    if (Application::getInstance()->useValidationLayer()) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(getValidationLayer().size());
        createInfo.ppEnabledLayerNames = getValidationLayer().data();
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(getPhysicalDevice(), &createInfo, nullptr, &m_device) != VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    vkGetDeviceQueue(m_device, indices.graphicsFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, indices.presentFamily.value(), 0, &m_presentQueue);
}

Application* Application::getInstance()
{
    static Application instance;
    return &instance;
}

VkInstance& Application::getVulkanInstance()
{
    return m_instance;
}

int Application::rateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR& surface)
{

    vkGetPhysicalDeviceProperties(device, &m_deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &m_deviceFeatures);

    bool extensionsSupported = checkDeviceExtensionSupport(device);
    QueueFamilyIndices familyQueue = findQueueFamilies(device, surface);

    int score = 0;

    // Discrete GPUs have a significant performance advantage
    if (m_deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }

    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device, surface);
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    // Maximum possible size of textures affects graphics quality
    score += m_deviceProperties.limits.maxImageDimension2D;

    // Application can't function without geometry shaders
    if (!m_deviceFeatures.geometryShader
        || !familyQueue.isComplete()
        || !extensionsSupported
        || !swapChainAdequate) {
        return 0;
        }

    return score;
}

SwapChainSupportDetails Application::querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface)
{
    SwapChainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.presentModes.data());
    }
    
    
    return details;
}

bool Application::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_deviceExtensions.begin(), m_deviceExtensions.end());

    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

QueueFamilyIndices Application::findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface)
{
    
    QueueFamilyIndices graphicsFamily;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            graphicsFamily.graphicsFamily = i;
        }
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

        if (presentSupport)
        {
            graphicsFamily.presentFamily = i;
        }
        if (graphicsFamily.isComplete()) break;
        i++;
    }

    return graphicsFamily;
    
}

bool Application::checkValidationSupport()
{

    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (const char* layerName : m_validationLayers) {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers) {
            if (strcmp(layerName, layerProperties.layerName) == 0) {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) {
            return false;
        }
    }

    return true;
}

std::vector<const char*> Application::getRequiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_enableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
}

VkDevice const& Application::getDevice()
{
    return m_device;
}

VkPhysicalDevice const& Application::getPhysicalDevice()
{
    return m_physicalDevice;
}

bool const& Application::useValidationLayer()
{
    return m_enableValidationLayers;
}

std::vector<const char*> const& Application::getDeviceExtensions()
{
    return m_deviceExtensions;
}

std::vector<const char*> const& Application::getValidationLayer()
{
    return m_validationLayers;
}

VkQueue const& Application::getGraphicQueue()
{
    return m_graphicsQueue;
}

VkQueue const& Application::getPresentQueue()
{
    return m_presentQueue;
}

VkPhysicalDeviceProperties& Application::GetPhysicalDeviceProperties()
{
    return m_deviceProperties;
}

VkPhysicalDeviceFeatures& Application::GetPhysicalDeviceFeatures()
{
    return m_deviceFeatures;
}

VkBool32 Application::debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                    VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                    void* pUserData)
{
    {

        // VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that is unrelated to the specification or performance
        // VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened that violates the specification or indicates a possible mistake
        // VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential non-optimal use of Vulkan

        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error, but very likely a bug in your application
        // VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause crashes

        if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
            std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        }

        return VK_FALSE;
    }
}

VkResult Application::CreateDebugUtilsMessengerEXT(VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

void Application::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}
