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
 
"Engine/stbi.h" 
"Engine/stbi.cpp" 

"Engine/Objects/Camera.h" 
"Game/TestScene.h" "Game/TestScene.cpp" "Engine/OpenGL/Mesh.h" "Engine/OpenGL/Mesh.cpp" "Engine/OpenGL/Model.h" "Engine/OpenGL/Model.cpp" "Engine/Objects/Base/Chunk.h")

add_executable(TheFrim ${SOURCE_FILES})

target_link_libraries(TheFrim glfw)
target_link_libraries(TheFrim assimp)

set_target_properties(TheFrim PROPERTIES 
VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
CMAKE_CXX_STANDARD 20
)