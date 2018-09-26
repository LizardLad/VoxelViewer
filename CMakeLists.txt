cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)
project(VoxelViewer)

# Find SDL2 and OpenGL
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/modules")
find_package(SDL2 REQUIRED)
cmake_policy(SET CMP0072 NEW)
find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)

# Define executable target
include_directories(${SDL2_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR} ${CMAKE_BINARY_DIR})
add_executable(VoxelViewer src/main.cpp src/key_mouse_manager.cpp src/physics.cpp src/render.cpp src/shader_utils.cpp src/vectors.cpp src/window_utils.cpp src/resources.cpp)
target_link_libraries(VoxelViewer ${SDL2_LIBRARIES} ${OPENGL_LIBRARIES} ${GLM_LIBRARIES} m)
