#======================================================================
# add_shader
#
# The SHADER variable contains the path to the shader file from the top
# level CMakeLists.txt. This shader is converted into a SPIR-V file and
# given the same path within the build directory.
#
# Usage:
# - Call from top level CMakeLists.txt
# - Pass in the path to the desired shader file as the SHADER variable
#======================================================================

function(add_shader SHADER)
  # Find glslc shader compiler
  find_program(GLSLC glslc)
  
  # Get path to shader file
  set(SHADER_PATH ${CMAKE_SOURCE_DIR}/${SHADER})
  
  # Create output path in build dir with shader as a SPIR-V file
  set(OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/${SHADER}.spv)
  
  # Create the directory for the sshader if it doesn't exist already
  get_filename_component(OUTPUT_DIR ${OUTPUT_PATH} DIRECTORY)
  file(MAKE_DIRECTORY ${OUTPUT_DIR})
  
  # Custom command to compile shader
  add_custom_command(
    OUTPUT ${OUTPUT_PATH}
    COMMAND ${GLSLC} -o ${OUTPUT_PATH} ${SHADER_PATH}
    DEPENDS ${SHADER_PATH}
    IMPLICIT_DEPENDS CXX ${SHADER_PATH}
    VERBATIM)
    
  # Ensure the build target depends on the generated output
  set_source_files_properties(${OUTPUT_PATH} PROPERTIES GENERATED TRUE)
  target_sources(${TARGET} PRIVATE ${OUTPUT_PATH})
endfunction(add_shader)
