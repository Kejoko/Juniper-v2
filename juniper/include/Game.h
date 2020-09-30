//======================================================================
// Game.h
//
// Keegan Kochis
// Created: 2020/9/30
// The declaration of the Game class.
//======================================================================

#include <GLFW/glfw3.h>

class Game {
public:
    // Struct for user settings (screen dimensions and stuff), for now will be here
    int mWindowWidth = 800;
    int mWindowHeight = 600;
    
    Game();
    
    void run();
    
private:
    GLFWwindow* mpWindow;
    
    void init_window();
    void init_vulkan();
    void main_loop();
    void clean_up();
};
