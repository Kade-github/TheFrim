cmake_minimum_required(VERSION 3.27)

if (WIN32)
    # /ENTRY:mainCRTStartup keeps the same "main" function instead of requiring "WinMain"
    set(SUBSYSTEM_LINKER_OPTIONS "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
endif()

project(
	TheFrim
	VERSION 1.0
	LANGUAGES C CXX
	DESCRIPTION "3D Voxel Story Game"
)

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
"Game/Objects/Base/UI/ItemUI.cpp"
 "Engine/Helpers/Collision2D.h" "Engine/Helpers/Collision2D.cpp" "Game/Data/Structures/Tree.cpp" "Game/Scenes/Worlds.h" "Game/Scenes/Worlds.cpp" "Game/Objects/Menu/World.h" "Game/Objects/Menu/World.cpp" "Game/Objects/Menu/InputBar.h" "Game/Objects/Menu/InputBar.cpp" "Game/Scenes/CreateWorld.h" "Game/Scenes/CreateWorld.cpp" "Game/Scenes/GeneratingWorld.h" "Game/Scenes/GeneratingWorld.cpp" "Game/Scenes/Gameplay.h" "Game/Scenes/Gameplay.cpp" "Game/Objects/Base/Player.h" "Game/Objects/Base/Player.cpp" "Game/Objects/Base/Entity.h" "Game/Objects/Base/Entity.cpp" "Engine/Objects/Camera.cpp"  "Engine/Objects/3DText.h" "Engine/Objects/3DText.cpp" "Game/Objects/Base/Blocks/Stone.h" "Engine/Helpers/MathTools.h" "Game/Objects/Base/Blocks/Cobblestone.h" "Game/LightingManager.h" "Game/LightingManager.cpp" "Game/Objects/Base/Blocks/Wood.h" "Game/Objects/Base/Blocks/Leaves.h" "Game/Data/Structure.h" "Game/Data/Structures/Tree.h" "Game/Objects/Menu/WorldButton.h" "Game/Objects/Menu/WorldButton.cpp" "Game/Scenes/LoadingWorld.h" "Game/Scenes/LoadingWorld.cpp" "Engine/Objects/2DRectangle.h" "Engine/Audio/AudioManager.h" "Game/MusicManager.h" "Game/MusicManager.cpp" "Engine/Audio/AudioManager.cpp"  "Game/Data/Player.h" "Game/Objects/Base/Hud.h" "Game/Objects/Base/Hud.cpp" "Game/Objects/Base/Blocks/Water.h" "Game/Objects/Base/Blocks/Sand.h" "Engine/TextureCache.h" "Engine/TextureCache.cpp" "Engine/External/Spritesheet.h" "Engine/External/Spritesheet.cpp" "Engine/Objects/3DSprite.h" "Engine/Objects/3DSprite.cpp" "Game/Objects/Base/UI/ItemUI.h" "Game/Objects/Base/UI/BoxUI.h" "Game/Objects/Base/UI/BoxUI.cpp" "Game/Objects/Base/UI/Inventory.h" "Game/Objects/Base/UI/Inventory.cpp" "Game/Objects/Base/DroppedItem.h" "Game/Objects/Base/DroppedItem.cpp" "Game/DroppedItemManager.h" "Game/DroppedItemManager.cpp" "Game/Scenes/SettingsMenu.h" "Game/Scenes/SettingsMenu.cpp" "Game/Data/Settings.h" "Game/Data/Settings.cpp" "Game/CraftingManager.h" "Game/CraftingManager.cpp" "Game/Objects/Base/Blocks/WoodenPlanks.h" "Game/Objects/Base/Blocks/CraftingTable.h" "Game/Objects/Base/Blocks/CraftingTable.cpp" "Game/Objects/Menu/DragBar.h" "Game/Objects/Menu/DragBar.cpp" "Game/Scenes/BlockTest.h" "Game/Scenes/BlockTest.cpp" "Game/Objects/Base/Blocks/Glass.h"

"app.rc" "Game/Objects/Base/Blocks/Water.cpp" "Game/Objects/Base/Blocks/Bedrock.h" "Game/Objects/Base/Blocks/IronOre.h" "Game/Objects/Base/Blocks/CoalOre.h" "Game/Objects/Base/Blocks/GoldOre.h" "Game/Objects/Base/Blocks/DiamondOre.h"
		Engine/Helpers/Performance.h
 "Game/Objects/Base/FallingBlock.h" "Game/Objects/Base/FallingBlock.cpp" "Game/Objects/Base/Blocks/Sand.cpp" "Game/Objects/Base/Blocks/NullBlock.h" "Game/Objects/Base/Blocks/Torch.h"  "Game/Objects/Base/Blocks/Torch.cpp" "Game/Objects/Base/Blocks/Furnace.h" "Game/Objects/Base/Blocks/Furnace.cpp"

 "Game/Objects/Base/Blocks/RuinedCobblestone.h"
 "Game/Objects/Base/Blocks/RuinedDebris.h" "Game/Objects/Base/Blocks/ReinforcedIronBlock.h" "Game/Objects/Base/Zombie.h" "Game/Objects/Base/AI.h" "Game/Objects/Base/AI.cpp" "Game/MobManager.h" "Game/MobManager.cpp" "Game/Objects/Base/Pig.h" "Game/Objects/Base/Pig.cpp" "Game/Data/Structures/Ruins.h" "Game/Data/Structures/Ruins.cpp" "Game/Objects/Base/Blocks/Rocket.h" "Game/Objects/Base/Blocks/Rocket.cpp" "Game/Objects/Base/RocketEnd.h" "Game/Objects/Base/RocketEnd.cpp" "Game/CreditsReader.h" "Game/CreditsReader.cpp")

link_directories(bassLibrary)

add_executable(TheFrim ${SOURCE_FILES} "Game/Objects/Base/Zombie.cpp")

target_link_options(TheFrim PRIVATE ${SUBSYSTEM_LINKER_OPTIONS})

set_target_properties(TheFrim PROPERTIES 
VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

target_compile_features(TheFrim PUBLIC cxx_std_20)

target_include_directories(TheFrim PUBLIC "${CMAKE_SOURCE_DIR}/../Include")
target_include_directories(TheFrim PUBLIC "${CMAKE_SOURCE_DIR}/Engine")

include_directories(${CMAKE_SOURCE_DIR}../Include)
include_directories(${CMAKE_SOURCE_DIR}../Lib)
include_directories(${CMAKE_SOURCE_DIR}../)

target_link_libraries(TheFrim bass)
target_link_libraries(TheFrim bass_fx)

find_package(pugixml CONFIG REQUIRED)
target_link_libraries(TheFrim pugixml::pugixml)

find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(TheFrim glfw)

find_package(assimp CONFIG REQUIRED)
target_link_libraries(TheFrim assimp::assimp)

add_subdirectory(zstr)
target_link_libraries(TheFrim zstr::zstr)