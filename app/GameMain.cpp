//======================================================================
// GameMain.cpp
//
// Keegan Kochis
// Created: 2020/9/30
// Entry point to the application created with the Juniper engine
//======================================================================

#include <cstdlib>

#include <iostream>
#include <stdexcept>

#include "Game.h"

int main() {
    Game game;
    
    try {
        game.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
