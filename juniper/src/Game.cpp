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





void Game::init_window() {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    mpWindow = glfwCreateWindow(mWindowWidth, mWindowHeight, "Juniper Game", nullptr, nullptr);
}





void Game::init_vulkan() {
    create_vulkan_instance();
    setup_vulkan_debug_messenger();
}





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
    
    std::vector<const char*> extensions = get_required_glfw_extensions();
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    if (mEnableValidationLayers) {
        createInfo.enabledLayerCount = static_cast<uint32_t>(mValidationLayers.size());
        createInfo.ppEnabledLayerNames = mValidationLayers.data();
    }
    else {
        createInfo.enabledLayerCount = 0;
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

void DestroyDebugUtilsMessengerEXT(VkInstance instance,
                                   VkDebugUtilsMessengerEXT debugMessenger,
                                   const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

void Game::setup_vulkan_debug_messenger() {
    if (!mEnableValidationLayers) return;
    
    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    // Update with desired message severities (all are enabled right now)
    createInfo.messageSeverity =
        VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
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
    
    // Load and call vkCreateDebugUtilsMessengerEXT function
    if (CreateDebugUtilsMessengerEXT(mVulkanInstance, &createInfo, nullptr, &mVulkanDebugMessenger) != VK_SUCCESS) {
        throw std::runtime_error("Failed to set up Vulkan debug messenger!");
    }
}







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





void Game::main_loop() {
    while (!glfwWindowShouldClose(mpWindow)) {
        glfwPollEvents();
    }
}





void Game::clean_up() {
    if (mEnableValidationLayers) {
        // Load and call vkDestroyDebugUtilsMessengerEXT
        DestroyDebugUtilsMessengerEXT(mVulkanInstance, mVulkanDebugMessenger, nullptr);
    }
    
    vkDestroyInstance(mVulkanInstance, nullptr);
    
    glfwDestroyWindow(mpWindow);

    glfwTerminate();
}
