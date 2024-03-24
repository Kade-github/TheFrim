cmake_minimum_required(VERSION 3.27)

project(
	TheFrim
	VERSION 1.0
	LANGUAGES C CXX
	DESCRIPTION "3D Voxel Story Game"
)

set(SOURCE_FILES 
main.cpp 
glad.c 
miniz.c
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
"Engine/External/imgui_impl_glfw.h"
"Engine/Objects/Camera.h" 
 
 
"Engine/OpenGL/Mesh.h" 
"Engine/OpenGL/Mesh.cpp" 
"Engine/OpenGL/Model.h" 
"Engine/OpenGL/Model.cpp" 
"Game/Objects/Base/Block.h" 
"Game/Objects/Base/Block.cpp" 
"Game/Objects/Base/Chunk.cpp" 
"Game/Objects/Base/Chunk.h" 
"Game/Objects/Base/Blocks/Grass.h" 
"Game/Objects/Base/Blocks/Dirt.h" 
"Game/Data/World.h"   
"Game/WorldManager.h" 
"Game/WorldManager.cpp" 
"Engine/External/imgui_impl_glfw.cpp" 
"Engine/External/imgui_impl_opengl3.cpp" 
"Engine/External/imgui_impl_opengl3.h" 
"Engine/External/imgui_impl_opengl3_loader.h"
"Game/Data/World.cpp" 
"Engine/Helpers/StringTools.h" 
"Engine/Objects/2DCamera.h" 
"Engine/Objects/2DCamera.cpp" 
"Engine/Objects/2DGameObject.h"  
"Engine/Objects/2DSprite.h" 
"Engine/Objects/2DSprite.cpp" 
"Engine/Font/Fnt.h" 
"Engine/Font/Fnt.cpp" 
"Engine/Objects/2DText.h" 
"Engine/Objects/2DText.cpp" 
"Game/Scenes/MainMenu.h" 
"Game/Scenes/MainMenu.cpp" 
"Game/Objects/Menu/Bar.h"
"Game/Objects/Menu/Bar.cpp"
 "Engine/Helpers/Collision2D.h" "Engine/Helpers/Collision2D.cpp" "Game/Data/Structures/Tree.cpp" "Game/Scenes/Worlds.h" "Game/Scenes/Worlds.cpp" "Game/Objects/Menu/World.h" "Game/Objects/Menu/World.cpp" "Game/Objects/Menu/InputBar.h" "Game/Objects/Menu/InputBar.cpp" "Game/Scenes/CreateWorld.h" "Game/Scenes/CreateWorld.cpp" "Game/Scenes/GeneratingWorld.h" "Game/Scenes/GeneratingWorld.cpp" "Game/Scenes/Gameplay.h" "Game/Scenes/Gameplay.cpp" "Game/Objects/Base/Player.h" "Game/Objects/Base/Player.cpp" "Game/Objects/Base/Entity.h" "Game/Objects/Base/Entity.cpp" "Engine/Objects/Camera.cpp"  "Engine/Objects/3DText.h" "Engine/Objects/3DText.cpp" "Game/Objects/Base/Blocks/Stone.h" "Engine/Helpers/MathTools.h" "Game/Objects/Base/Blocks/Cobblestone.h" "Game/LightingManager.h" "Game/LightingManager.cpp" "Game/Objects/Base/Blocks/Wood.h" "Game/Objects/Base/Blocks/Leaves.h" "Game/Data/Structure.h" "Game/Data/Structures/Tree.h" "Game/Objects/Menu/WorldButton.h" "Game/Objects/Menu/WorldButton.cpp" "Game/Scenes/LoadingWorld.h" "Game/Scenes/LoadingWorld.cpp" "Engine/Objects/2DRectangle.h" "Engine/Audio/AudioManager.h" "Game/MusicManager.h" "Game/MusicManager.cpp" "Engine/Audio/AudioManager.cpp")

link_directories(bassLibrary)

add_executable(TheFrim ${SOURCE_FILES})

if (WIN32)
    # /ENTRY:mainCRTStartup keeps the same "main" function instead of requiring "WinMain"
    set(SUBSYSTEM_LINKER_OPTIONS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
else()
    set(SUBSYSTEM_LINKER_OPTIONS "-mwindows")
endif()

set_target_properties(TheFrim PROPERTIES 
VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

target_compile_features(TheFrim PUBLIC cxx_std_20)

target_include_directories(TheFrim PUBLIC "${CMAKE_SOURCE_DIR}/../Include")
target_include_directories(TheFrim PUBLIC "${CMAKE_SOURCE_DIR}/Engine")

include_directories(${CMAKE_SOURCE_DIR}../Include)
include_directories(${CMAKE_SOURCE_DIR}../Lib)

target_link_libraries(TheFrim bass)
target_link_libraries(TheFrim bass_fx)

find_package(pugixml CONFIG REQUIRED)
target_link_libraries(TheFrim pugixml::shared pugixml::pugixml)

find_package(imgui CONFIG REQUIRED)
target_link_libraries(TheFrim imgui::imgui)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(TheFrim glfw)

find_package(miniz CONFIG REQUIRED)
target_link_libraries(TheFrim miniz::miniz)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(TheFrim assimp::assimp)

add_subdirectory(zstr)
target_link_libraries(TheFrim zstr::zstr)