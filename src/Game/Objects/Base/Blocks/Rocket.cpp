#include "Rocket.h"
#include "../../../LightingManager.h"
#include "../../../Scenes/Gameplay.h"
#include <Game.h>
#include "../RocketEnd.h"

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
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	if (position.y < highest)
	{

		gp->hud->ShowHint("Route blocked, unable to launch.");
		Hud::endSequence = false;
		gp->hud->playEnd = false;
		return true;
	}

	if (LightingManager::GetInstance()->sun.angle < 200 || LightingManager::GetInstance()->sun.angle > 290)
	{
		gp->hud->ShowHint("Route unclear, the sky must be shrouded in darkness.");
		Hud::endSequence = false;
		gp->hud->playEnd = false;
		return true;
	}

	if (!destroyed)
	{
		RocketEnd* end = new RocketEnd(position);

		gp->AddObject(end);

		destroyed = true;
		currentChunk->ModifyBlock(position.x, position.y, position.z, 0);
	}

	return true;
}
