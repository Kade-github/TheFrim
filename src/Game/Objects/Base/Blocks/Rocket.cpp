#include "Rocket.h"
#include "../../../LightingManager.h"
#include "../../../Scenes/Gameplay.h"
#include <Game.h>

Rocket::Rocket(glm::vec3 _position) : Block(_position, BlockType::ROCKET)
{
	position = _position;

	soundType = SoundType::S_STONE;
	toughness = 0.3f;

	isModel = true;
	transparent = true;
	isInteractable = true;
	updateable = true;

	currentChunk = WorldManager::instance->GetChunk(position.x, position.z);
}

void Rocket::LoadModel()
{
	m = Model("Assets/Models/rocket_frim.obj");
	m.LoadUVMap("rocket_frim");
	m.scale = glm::vec3(0.6f, 0.8f, 0.6f);
}

void Rocket::Mo()
{
	m.position += glm::vec3(0.5f, 0, 0.5f);
}

void Rocket::OnInteract()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	gp->hud->ShowRocketScreen();
}

bool Rocket::Update(int tick)
{
	if (!Hud::endSequence)
		return true;

	int highest = currentChunk->GetHighestBlock(position.x, position.z);

	if (position.y < highest)
	{
		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		gp->hud->ShowHint("Route blocked, unable to launch.");
		Hud::endSequence = false;
		return true;
	}

	return true;
}
