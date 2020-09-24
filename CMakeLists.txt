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
# Subdirectories
#======================================================================
add_subdirectory(juniper)

#======================================================================
# Juniper Entry Point
#======================================================================
add_executable(JuniperEngine Juniper/EntryPoint.cpp)

target_link_libraries(
  JuniperEngine
  PRIVATE
  ${J_LIBS})
