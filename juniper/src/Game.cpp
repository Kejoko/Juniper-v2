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
#include <stdexcept>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
    pick_physical_device();
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
        
        if (!layerFound) return false;
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
    
    std::multimap<int, VkPhysicalDevice> candidates;
    for (const auto& device : devices) {
        int score = rate_physical_device_suitability(device);
        candidates.insert(std::make_pair(score, device));
    }
    
    if ( candidates.rbegin()->first > 0) {
        mPhysicalDevice = candidates.rbegin()->second;
    }
    else {
        throw std::runtime_error("Failed to find GPUs with Vulkan support!");
    }
}

//------------------------------------------------------------------------------------------
// Find the queue families that are used for submitting operations to Vulkan
//------------------------------------------------------------------------------------------
Game::QueueFamilyIndices Game::find_queue_families(VkPhysicalDevice device) {
    QueueFamilyIndices indices;
    
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    
    // Throw error if physical device has no queue families, if queueFamilyCount == 0
    
    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
    
    for (uint32_t i = 0; i < queueFamilyCount; i++) {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.foundGraphicsFamily = true;
            indices.graphicsFamily = i;
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
    
    return indices.is_complete();
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
    if (mEnableValidationLayers) {
        // Load and call vkDestroyDebugUtilsMessengerEXT
        DestroyDebugUtilsMessengerEXT(mVulkanInstance, mVulkanDebugMessenger, nullptr);
    }
    
    vkDestroyInstance(mVulkanInstance, nullptr);
    
    glfwDestroyWindow(mpWindow);

    glfwTerminate();
}
