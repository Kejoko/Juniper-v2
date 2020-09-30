#======================================================================
# Juniper
#======================================================================
cmake_minimum_required(VERSION 3.14)
project(Juniper VERSION 0.1 LANGUAGES CXX)

#======================================================================
# C++ 11 Support
#======================================================================
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#======================================================================
# GLFW
#======================================================================
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(juniper/dependencies/glfw)

list(APPEND DEP_LIBS glfw)

#======================================================================
# Vulkan
#======================================================================
link_directories(
  ${PROJECT_SOURCE_DIR}/juniper/dependencies/vulkansdk/macOS/lib)

list(APPEND DEP_LIBS libvulkan.1.dylib libvulkan.1.2.148.dylib)

#======================================================================
# Include project dependencies
#======================================================================
include_directories(
  ${PROJECT_SOURCE_DIR}/juniper/dependencies/glfw/include
  ${PROJECT_SOURCE_DIR}/juniper/dependencies/glm
  ${PROJECT_SOURCE_DIR}/juniper/dependencies/vulkansdk/macOS/include)

#======================================================================
# Subdirectories, include directories, Juniper libraries
#======================================================================
add_subdirectory(app)
add_subdirectory(juniper)

include_directories("${PROJECT_SOURCE_DIR}/juniper/include")

list(APPEND J_LIBS J_Game)

#======================================================================
# Juniper Entry Point
#======================================================================
add_executable(JuniperEngine juniper/JuniperMain.cpp)

target_link_libraries(
  JuniperEngine
  PRIVATE
  ${DEP_LIBS}
  ${J_LIBS})

#======================================================================
# Game Entry Point
#======================================================================
add_executable(Game app/GameMain.cpp)

target_link_libraries(
  Game
  PRIVATE
  ${DEP_LIBS}
  ${J_LIBS})