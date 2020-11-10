//======================================================================
// Game.h
//
// Keegan Kochis
// Created: 2020/9/30
// The declaration of the Game class.
//======================================================================

#ifndef GAME_H
#define GAME_H

#include <optional>
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
    // Struct for user settings (screen dimensions and stuff) will be member vars for now
    int mWindowWidth = 800;
    int mWindowHeight = 600;
    
    
    struct QueueFamilyIndices_t {
        bool foundGraphicsFamily = false;
        bool foundPresentFamily = false;
        uint32_t graphicsFamily;
        uint32_t presentFamily;
        bool is_complete() {
            return foundGraphicsFamily && foundPresentFamily;
        }
    }; typedef QueueFamilyIndices_t QueueFamilyIndices;
    
    
    struct SwapChainSupportDetails_t {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
        bool is_complete() {
            return !formats.empty() && !presentModes.empty();
        }
    }; typedef SwapChainSupportDetails_t SwapChainSupportDetails;
    
    
    Game();
    
    void run();
    
private:
    
    GLFWwindow* mpWindow;
    VkInstance mVulkanInstance;
    VkDebugUtilsMessengerEXT mVulkanDebugMessenger;
    VkPhysicalDevice mPhysicalDevice = VK_NULL_HANDLE;
    VkDevice mDevice;
    VkQueue mGraphicsQueue;
    VkSurfaceKHR mWindowSurface;
    VkQueue mPresentQueue;
    VkSwapchainKHR mSwapchain;
    std::vector<VkImage> mSwapchainImages;
    VkFormat mSwapchainImageFormat;
    VkExtent2D mSwapchainExtent;
    std::vector<VkImageView> mSwapchainImageViews;
    
    std::vector<const char*> mValidationLayers = { "VK_LAYER_KHRONOS_validation" };
    const bool mEnableValidationLayers = DEBUG_ON;
    
    void init_window();
    void init_vulkan();
    void create_vulkan_instance();
    bool check_vulkan_validation_layer_support();
    std::vector<const char*> get_required_glfw_extensions();
    void setup_vulkan_debug_messenger();
    void populate_vulkan_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void create_surface();
    SwapChainSupportDetails query_swap_chain_support(VkPhysicalDevice device);
    VkSurfaceFormatKHR choose_swap_surface_format(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR choose_swap_present_mode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D choose_swap_extent(const VkSurfaceCapabilitiesKHR& capabilities);
    void create_swap_chain();
    void pick_physical_device();
    QueueFamilyIndices find_queue_families(VkPhysicalDevice device);
    bool is_device_suitable(VkPhysicalDevice device);
    bool check_device_extension_support(VkPhysicalDevice device);
    void create_logical_device();
    void create_image_views();
    void main_loop();
    void clean_up();
    
    static VKAPI_ATTR VkBool32 VKAPI_CALL
    vulkan_debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT messgaeSeverity,
                          VkDebugUtilsMessageTypeFlagsEXT messageType,
                          const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                          void* pUserData);
};

#endif // GAME_H
