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

include_directories(
  ${PROJECT_SOURCE_DIR}/juniper/dependencies/glfw/include)

add_subdirectory(juniper/dependencies/glfw)

list(APPEND DEP_LIBS glfw)

#======================================================================
# Vulkan
#======================================================================
find_package(Vulkan REQUIRED)

include_directories(${Vulkan_INCLUDE_DIRS})
List(APPEND DEP_LIBS ${Vulkan_LIBRARIES})

#======================================================================
# GLM
#======================================================================
include_directories(${PROJECT_SOURCE_DIR}/juniper/dependencies/glm)

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
# Juniper Shaders
#======================================================================
include(cmake/add_shader.cmake)

add_shader(JuniperEngine juniper/src/shaders/shader.vert)
add_shader(JuniperEngine juniper/src/shaders/shader.frag)

#======================================================================
# Game Entry Point
#======================================================================
add_executable(Game app/GameMain.cpp)

target_link_libraries(
  Game
  PRIVATE
  ${DEP_LIBS}
  ${J_LIBS})