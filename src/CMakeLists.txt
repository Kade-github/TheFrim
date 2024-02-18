cmake_minimum_required(VERSION 3.1...3.28)

project(
	TheFrim
	VERSION 1.0
	LANGUAGES C CXX
	DESCRIPTION "3D Voxel Story Game"
)

include_directories(${CMAKE_SOURCE_DIR}../Include)
include_directories(${CMAKE_SOURCE_DIR}../Lib)

find_package(OpenGL REQUIRED)

find_path(
  GLFW_INCLUDE_DIR
  NAMES
  GLFW
  PATHS
  include)

find_library(
  GLFW_LIBRARY
  NAMES
  glfw glfw3 glfw3dll
  PATHS
  lib)

set(SOURCE_FILES 
main.cpp 
glad.c 
"Engine/Game.h" 
"Engine/Game.cpp" 
"Engine/Logging.h" 
"Engine/Objects/Scene.h" 
"Engine/Objects/GameObject.h" 
"Engine/OpenGL/Texture.h" 
"Engine/OpenGL/Texture.cpp" 
"Engine/OpenGL/Shader.h" 
"Engine/OpenGL/Shader.cpp" 
"Engine/OpenGL/Renderer.h" 
"Engine/stbi.h" 
"Engine/stbi.cpp" 
"Engine/OpenGL/Renderer.cpp"
"Engine/Objects/Camera.h" 
"Engine/Objects/Base/Cube.h"
"Engine/Objects/Base/Cube.cpp"
 "Game/TestScene.h" "Game/TestScene.cpp")

add_executable(TheFrim ${SOURCE_FILES})

target_link_libraries(TheFrim glfw3)

set_target_properties(TheFrim PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")