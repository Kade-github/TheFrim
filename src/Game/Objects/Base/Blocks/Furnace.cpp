#include "Furnace.h"
#include "../../../Scenes/Gameplay.h"
#include <Game.h>

void Furnace::OnInteract()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	gp->player->ToggleFurnace();
}

bool Furnace::Update(int tick)
{
	return true;
}
