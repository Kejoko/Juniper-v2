//======================================================================
// Entry.cpp
//
// Keegan Kochis
// Created: 2020/9/24
// Entry point to the application created with the Juniper engine
//======================================================================

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

int main(int argc, char* argv[]) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* pJuniperWindow = glfwCreateWindow(800, 600, "Juniper Engine", nullptr, nullptr);

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

    std::cout << extensionCount << " extensions supported\n";

    glm::mat4 matrix;
    glm::vec4 vec;
    auto test = matrix * vec;

    while(!glfwWindowShouldClose(pJuniperWindow)) {
        glfwPollEvents();
    }

    glfwDestroyWindow(pJuniperWindow);

    glfwTerminate();

    return 0;
}
