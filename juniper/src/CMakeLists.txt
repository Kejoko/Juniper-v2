#======================================================================
# Juniper/juniper/src
#======================================================================
add_library(
  J_Game
  Game.cpp
  ${J_INCLUDE_DIR}/Game.h)
target_include_directories(J_Game PUBLIC "${J_INCLUDE_DIR}")