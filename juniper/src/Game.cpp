//======================================================================
// Game.cpp
//
// Keegan Kochis
// Created: 2020/9/30
// The definition of the Game class.
//======================================================================

#include "Game.h"

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
    
}

void Game::main_loop() {
    while (!glfwWindowShouldClose(mpWindow)) {
        glfwPollEvents();
    }
}

void Game::clean_up() {
    glfwDestroyWindow(mpWindow);

    glfwTerminate();
}
