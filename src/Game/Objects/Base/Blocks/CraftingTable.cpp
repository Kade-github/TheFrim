#include "CraftingTable.h"
#include "../../../Scenes/Gameplay.h"
#include <Game.h>

void CraftingTable::OnInteract()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	gp->player->ToggleCraftingTable();
}
