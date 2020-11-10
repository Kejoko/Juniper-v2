//======================================================================
// Game.cpp
//
// Keegan Kochis
// Created: 2020/9/30
// The definition of the Game class.
//======================================================================

#include "Game.h"

#include <cstring>
#include <iostream>
#include <map>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Game::Game() {}

void Game::run() {
    init_window();
    init_vulkan();
    main_loop();
    clean_up();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Game::init_window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mpWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Juniper Game", nullptr, nullptr);
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Game::init_vulkan() {
    create_vulkan_instance();
    setup_vulkan_debug_messenger();
    create_surface();
    pick_physical_device();
    create_logical_device();
}

//------------------------------------------------------------------------------------------
// Create and populate the application information and instance information structs to pass
// to the vulkan instance member variable upon creation attempt
//------------------------------------------------------------------------------------------
void Game::create_vulkan_instance() {
    if (mEnableValidationLayers && !check_vulkan_validation_layer_support()) {
        throw std::runtime_error("Vulkan validation layers requested but not available!");
    }
    
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Juniper Game Name";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.pEngineName = "Juniper";
    applicationInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;
    applicationInfo.pNext = nullptr;
    
    std::vector<const char*> extensions = get_required_glfw_extensions();
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    if (mEnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
        
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
        populate_vulkan_debug_messenger_create_info(debugCreateInfo);
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
    }
    else {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }
    
    uint32_t availableExtensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, nullptr);
    
    if (availableExtensionCount == 0) {
        throw std::runtime_error("No extensions supported!");
    }
    
    std::vector<VkExtensionProperties> availableExtensions(availableExtensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionCount, availableExtensions.data());
    
    std::cout << "Available extensions:\n";
    for (const auto& extension : availableExtensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    
    if(vkCreateInstance(&createInfo, nullptr, &mVulkanInstance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }
    else {
        std::cout << "Successfully created Vulkan instance.\n";
    }
}

//------------------------------------------------------------------------------------------
// Check that all of the requested validation layers are available
//------------------------------------------------------------------------------------------
bool Game::check_vulkan_validation_layer_support() {
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    
    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    // Ensure all layers in mValidationLayers exist in availableLayers
    for (const char* layerName : mValidationLayers) {
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

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
std::vector<const char*> Game::get_required_glfw_extensions() {
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    std::vector<const char*> extensions;
    for (size_t i = 0; i < glfwExtensionCount; i++) {
        extensions.push_back(glfwExtensions[i]);
    }
    
    if (mEnableValidationLayers) {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }
    
    return extensions;
}

//------------------------------------------------------------------------------------------
// Attemp to load and return the vkCreateDebugUtilsMessengerEXT function
//------------------------------------------------------------------------------------------
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance,
                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                      const VkAllocationCallbacks* pAllocator,
                                      VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");

    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

//------------------------------------------------------------------------------------------
// Attemp to load and return the vkDestroyDebugUtilsMessengerEXT function
//------------------------------------------------------------------------------------------
void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Game::setup_vulkan_debug_messenger() {
    if (!mEnableValidationLayers) return;
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    populate_vulkan_debug_messenger_create_info(createInfo);
    
    // Load and call vkCreateDebugUtilsMessengerEXT function
    if (CreateDebugUtilsMessengerEXT(mVulkanInstance, &createInfo, nullptr, &mVulkanDebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up Vulkan debug messenger!");
    }
}

//------------------------------------------------------------------------------------------
// Populate vulkan debug messenger information struct
//------------------------------------------------------------------------------------------
void Game::populate_vulkan_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // Update with desired message severities (all are enabled right now)
    createInfo.messageSeverity =
//        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    // Update with desired message types (all are enabled right now)
    createInfo.messageType =
        VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = vulkan_debug_callback;
    createInfo.pUserData = nullptr; // Can be modified? Can pass a pointer to this application
}

//------------------------------------------------------------------------------------------
// Create callback function for Vulkan to use for debugging messages
//------------------------------------------------------------------------------------------
VKAPI_ATTR VkBool32 VKAPI_CALL
Game::vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messgaeSeverity,
                            VkDebugUtilsMessageTypeFlagsEXT messageType,
                            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                            void* pUserData) {
    // Only show this message if it is severe enough, determined by DEBUG or RELEASE (etc...) defines
    // Show more information than just the message. Include pObjects (etc...)
    // More detail here:
    // https://vulkan-tutorial.com/Drawing_a_triangle/Setup/Validation_layers#page_Message-callback
    std::cerr << "Validation layer: " << (*pCallbackData).pMessage << std::endl;
    
    return VK_FALSE;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
Game::SwapChainSupportDetails Game::query_swap_chain_support(VkPhysicalDevice device) {
    SwapChainSupportDetails details;
    
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mWindowSurface, &details.capabilities);
    
    uint32_t formatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, mWindowSurface, &formatCount, nullptr);
    
    if (formatCount != 0) {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, mWindowSurface, &formatCount, details.formats.data());
    }
    
    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, mWindowSurface, &presentModeCount, nullptr);
    
    if (presentModeCount != 0) {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, mWindowSurface, &presentModeCount, details.presentModes.data());
    }
    
    return details;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Game::create_surface() {
    if (glfwCreateWindowSurface(mVulkanInstance, mpWindow, nullptr, &mWindowSurface) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create window surface!");
    }
}

//------------------------------------------------------------------------------------------
// Choose physical device for Vulkan to use
// Rate the physical devices and choose the best/most suitable one
//------------------------------------------------------------------------------------------
int rate_physical_device_suitability(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    
    // This is not supported on Mac
//    if (!deviceFeatures.geometryShader) {
//        return 0;
//    }
    
    int score = 0;
    
    // Give preference to physical GPUs
    if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        score += 1000;
    }
    
    // Give preference to devices with larger possible texture sizes
    score += deviceProperties.limits.maxImageDimension2D;
    
    return score;
}

void Game::pick_physical_device() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, nullptr);
    
    if (deviceCount == 0) {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
    
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(mVulkanInstance, &deviceCount, devices.data());
    
    for (const auto& device : devices) {
        if (is_device_suitable(device)) {
            mPhysicalDevice = device;
            break;
        }
    }
    
    if (mPhysicalDevice == VK_NULL_HANDLE) {
        throw std::runtime_error("Failed to find a suitable GPU!");
    }
    
//              Rate devices and choose best candidate
//
//    std::multimap<int, VkPhysicalDevice> candidates;
//    for (const auto& device : devices) {
//        int score = rate_physical_device_suitability(device);
//        candidates.insert(std::make_pair(score, device));
//    }
//
//    if ( candidates.rbegin()->first > 0) {
//        mPhysicalDevice = candidates.rbegin()->second;
//    }
//    else {
//        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
//    }
}

//------------------------------------------------------------------------------------------
// Find the queue families that are used for submitting operations to Vulkan
//------------------------------------------------------------------------------------------
Game::QueueFamilyIndices Game::find_queue_families(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    if (queueFamilyCount == 0) {
        throw std::runtime_error("Physical device has no queue families!");
    }
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.foundGraphicsFamily = true;
            indices.graphicsFamily = i;
        }
        
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mWindowSurface, &presentSupport);
        
        if (presentSupport) {
            indices.foundPresentFamily = true;
            indices.presentFamily = i;
        }
        
        if (indices.is_complete()) {
            break;
        }
    }
    
    return indices;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
bool Game::is_device_suitable(VkPhysicalDevice device) {
    QueueFamilyIndices indices = find_queue_families(device);
    
    bool extensionsSupported = check_device_extension_support(device);
    
    bool swapChainAdequate = false;
    if (extensionsSupported) {
        SwapChainSupportDetails swapChainSupport = query_swap_chain_support(device);
        swapChainAdequate = swapChainSupport.is_complete();
    }
    
    return indices.is_complete() && extensionsSupported && swapChainAdequate;
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
bool Game::check_device_extension_support(VkPhysicalDevice device) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
    
    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
    
    std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());
    
    for (const auto& extension : availableExtensions) {
        requiredExtensions.erase(extension.extensionName);
    }
    
    return requiredExtensions.empty();
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Game::create_logical_device() {
    QueueFamilyIndices indices = find_queue_families(mPhysicalDevice);
    
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
    
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
    createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    createInfo.ppEnabledExtensionNames = deviceExtensions.data();
    
    if (mEnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
    }
    
    if (vkCreateDevice(mPhysicalDevice, &createInfo, nullptr, &mDevice) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }
    
    vkGetDeviceQueue(mDevice, indices.graphicsFamily, 0, &mGraphicsQueue);
    vkGetDeviceQueue(mDevice, indices.presentFamily, 0, &mPresentQueue);
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Game::main_loop() {
    while (!glfwWindowShouldClose(mpWindow)) {
        glfwPollEvents();
    }
}

//------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
void Game::clean_up() {
    vkDestroyDevice(mDevice, nullptr);
    
    if (mEnableValidationLayers) {
        // Load and call vkDestroyDebugUtilsMessengerEXT
        DestroyDebugUtilsMessengerEXT(mVulkanInstance, mVulkanDebugMessenger, nullptr);
    }
    
    vkDestroySurfaceKHR(mVulkanInstance, mWindowSurface, nullptr);
    
    vkDestroyInstance(mVulkanInstance, nullptr);
    
    glfwDestroyWindow(mpWindow);

    glfwTerminate();
}
