//======================================================================
// Game.h
//
// Keegan Kochis
// Created: 2020/9/30
// The declaration of the Game class.
//======================================================================

#ifndef GAME_H
#define GAME_H

#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define DEBUG

#ifdef DEBUG
#define DEBUG_ON true
#else
#define DEBUG_ON false
#endif

class Game {
public:
    // Struct for user settings (screen dimensions and stuff), for now will be here
    int mWindowWidth = 800;
    int mWindowHeight = 600;
    
    Game();
    
    void run();
    
private:
    GLFWwindow* mpWindow;
    VkInstance mVulkanInstance;
    VkDebugUtilsMessengerEXT mVulkanDebugMessenger;
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    
    std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };
    const bool mEnableValidationLayers = DEBUG_ON;
    
    void init_window();
    void init_vulkan();
    void create_vulkan_instance();
    bool check_vulkan_validation_layer_support();
    std::vector<const char*> get_required_glfw_extensions();
    void setup_vulkan_debug_messenger();
    void populate_vulkan_debug_messenger_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void pick_physical_device();
    void main_loop();
    void clean_up();
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messgaeSeverity,
                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                          void* pUserData);
};

#endif // GAME_H
