//======================================================================
// Game.cpp
//
// Keegan Kochis
// Created: 2020/9/30
// The definition of the Game class.
//======================================================================

#include "Game.h"

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

void Game::create_vulkan_instance() {
    VkApplicationInfo applicationInfo{};
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pApplicationName = "Juniper Game Name";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.pEngineName = "Juniper";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;
    
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions;
    glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &applicationInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;
    createInfo.enabledLayerCount = 0;
    
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    
    std::vector<VkExtensionProperties> extensions(extensionCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
    std::cout << "Available extensions:\n";
    for (const auto& extension : extensions) {
        std::cout << '\t' << extension.extensionName << '\n';
    }
    
    if(vkCreateInstance(&createInfo, nullptr, &mVulkanInstance) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create Vulkan instance!");
    }
}

void Game::init_vulkan() {
    create_vulkan_instance();
}

void Game::main_loop() {
    // Should run at constant timestep for physics
    while (!glfwWindowShouldClose(mpWindow)) {
        glfwPollEvents();
    }
}

void Game::clean_up() {
    vkDestroyInstance(mVulkanInstance, nullptr);
    
    glfwDestroyWindow(mpWindow);

    glfwTerminate();
}
