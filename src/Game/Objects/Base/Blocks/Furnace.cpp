    #include "Furnace.h"
#include "../../../Scenes/Gameplay.h"
#include <Game.h>

void Furnace::OnInteract()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	gp->player->ToggleFurnace();
}

int Furnace::GetTicksForItem(int item)
{
	switch (item)
	{
	case Data::ITEM_COBBLESTONE:
		return 10;
	default:
		return -1;
	}
}

int Furnace::GetOutputForItem(int item)
{
	switch (item)
	{
		case Data::ITEM_COBBLESTONE:
			return Data::ITEM_STONE;
		default:
			return Data::ITEM_NULL;
	}
}

bool Furnace::Update(int tick)
{
	if (tick % 20 != 0)
		return true;

	Data::DataTag ticksLeft = data.GetTag("ticksLeft");

	if (!ticksLeft.IsReal())
		return true;

	int ticks = std::stoi(ticksLeft.value);

	if (ticks <= -1)
	{
		// decide how long it should take

		Data::DataTag cooking = data.GetTag("cooking");

		if (!cooking.IsReal())
			return true;

		int itemType = std::stoi(cooking.value);

		int ticksForItem = GetTicksForItem(itemType);

		ticksNeeded = ticksForItem;
	

		data.SetTag("ticksLeft", std::to_string(ticksForItem));
		return true;
	}

	ticks--;

	tickPerc = 1.0f - ((float)ticks / (float)ticksNeeded);

	if (ticks == 0)
	{
		Data::DataTag cooking = data.GetTag("cooking");

		if (!cooking.IsReal())
			return true;

		int itemType = std::stoi(cooking.value);

		int output = GetOutputForItem(itemType);

		if (output == Data::ITEM_NULL)
			return true;

		int outputCount = data.GetTag("outputCount").IsReal() ? std::stoi(data.GetTag("outputCount").value) : 0;

		bool wasEmpty = data.GetTag("output").IsReal() ? false : true;

		data.SetTag("output", std::to_string(output));

		if (!wasEmpty)
		{
			outputCount++;
			data.SetTag("outputCount", std::to_string(outputCount));
		}
		else
		{
			data.SetTag("outputCount", "1");
		}

		int fuelCount = data.GetTag("fuelCount").IsReal() ? std::stoi(data.GetTag("fuelCount").value) : 0;

		if (fuelCount > 0)
		{
			fuelCount--;
			data.SetTag("fuelCount", std::to_string(fuelCount));
		}

		if (fuelCount == 0)
			data.SetTag("fuel", "-1");

		int cookingCount = data.GetTag("cookingCount").IsReal() ? std::stoi(data.GetTag("cookingCount").value) : 0;

		if (cookingCount > 0)
		{
			cookingCount--;
			data.SetTag("cookingCount", std::to_string(cookingCount));
		}

		if (cookingCount == 0)
			data.SetTag("cooking", "-1");

		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		if (gp->hud->inv->shown && gp->hud->inv->isFurnace)
		{
			gp->hud->inv->CreateInventory();
			gp->hud->inv->CreateFurnace();
			gp->hud->inv->UpdateInventory();
		}

		tickPerc = 0.0f;
		data.SetTag("ticksLeft", "-1");
	}
	else
	{
		data.SetTag("ticksLeft", std::to_string(ticks));
	}

	return true;
}
