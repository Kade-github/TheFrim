#include "Furnace.h"
#include "../../../Scenes/Gameplay.h"
#include <Game.h>
#include "../../../LightingManager.h"


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
	case Data::ITEM_IRON_ORE:
		return 20;
	case Data::ITEM_GOLD_ORE:
		return 20;
	case Data::ITEM_RUINED_DEBRIS:
		return 45;
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
	case Data::ITEM_IRON_ORE:
		return Data::ITEM_IRON_INGOT;
	case Data::ITEM_GOLD_ORE:
		return Data::ITEM_GOLD_INGOT;
	case Data::ITEM_COAL_ORE:
		return Data::ITEM_COAL;
	case Data::ITEM_RUINED_DEBRIS:
	{
		int random = rand() % 100;

		if (random < 30)
			return Data::ITEM_COAL;
		else if (random < 60)
			return Data::ITEM_IRON_ORE;
		else if (random < 90)
			return Data::ITEM_GOLD_ORE;
		else
			return Data::ITEM_DIAMOND;
	}
	case Data::ITEM_SAND:
		return Data::ITEM_GLASS;
	case Data::ITEM_UNCOOKED_PORK:
		return Data::ITEM_COOKED_PORK;
	default:
		return Data::ITEM_NULL;
	}
}

bool Furnace::Update(int tick)
{
	if (check)
	{
		if (data.GetTag("ticksLeft").IsReal())
		{
			int left = std::stoi(data.GetTag("ticksLeft").value);

			if (left >= 0)
			{
				cookingType = std::stoi(data.GetTag("cooking").value);
				ticksNeeded = GetTicksForItem(cookingType);
				ticks = left;

				LightingManager::GetInstance()->AddLight(position, 10);
				light = true;
			}
		}
		check = false;
	}

	if (tick % 20 != 0)
		return true;

	Data::DataTag ticksLeft = data.GetTag("ticksLeft");

	tickPerc = 0.0f;

	if (!ticksLeft.IsReal())
	{
		data.AddTag("ticksLeft", "-1");
		return true;
	}

	if (ticksNeeded <= -1 && ticks <= -1)
	{
		// decide how long it should take

		Data::DataTag cooking = data.GetTag("cooking");

		if (!cooking.IsReal())
			return true;

		Data::DataTag fuel = data.GetTag("fuel");

		if (!fuel.IsReal())
			return true;

		if (std::stoi(fuel.value) != Data::ITEM_COAL && std::stoi(fuel.value) != Data::ITEM_CONDENSED_COAL)
			return true;

		bool update = false;

		int cookingCount = data.GetTag("cooking_count").IsReal() ? std::stoi(data.GetTag("cooking_count").value) : 0;

		cookingType = std::stoi(cooking.value);

		if (cookingCount == 0 || cookingType == Data::ITEM_NULL)
			return true;

		int fuelCount = data.GetTag("fuel_count").IsReal() ? std::stoi(data.GetTag("fuel_count").value) : 0;

		if (fuelCount > 0)
		{
			fuelCount--;
			data.SetTag("fuel_count", std::to_string(fuelCount));
			update = true;
		}

		if (fuelCount == 0)
		{
			data.SetTag("fuel", "-1");
			data.SetTag("fuel_count", "0");
			update = true;
		}

	
		if (cookingCount > 0)
		{
			cookingCount--;
			data.SetTag("cooking_count", std::to_string(cookingCount));
			update = true;
		}

		if (cookingCount == 0)
		{
			data.SetTag("cooking", "-1");
			update = true;
		}

		if (update)
		{
			Gameplay* gp = (Gameplay*)Game::instance->currentScene;

			if (gp->hud->inv->shown && gp->hud->inv->isFurnace)
			{
				gp->hud->inv->CreateInventory();
				gp->hud->inv->CreateFurnace();
				gp->hud->inv->UpdateInventory();
			}
		}

		int itemType = std::stoi(cooking.value);

		int ticksForItem = GetTicksForItem(itemType);

		ticksNeeded = ticksForItem;
		ticks = ticksForItem;

		data.SetTag("ticksLeft", std::to_string(ticksNeeded));
		return true;
	}
	else
	{
		ticks--;
		tickPerc = 1.0f - ((float)ticks / (float)ticksNeeded);
	}



	if (ticks <= 0)
	{
		int itemType = cookingType;

		int output = GetOutputForItem(itemType);

		Data::DataTag out = data.GetTag("output");

		if (output == Data::ITEM_NULL)
			return true;

		if (out.IsReal() && output != std::stoi(out.value) && std::stoi(out.value) != 0)
			return true; // wait for the output to be taken

		int outputCount = data.GetTag("outputCount").IsReal() ? std::stoi(data.GetTag("outputCount").value) : 0;

		bool wasEmpty = data.GetTag("output").IsReal() ? false : true;

		int o = 0;

		if (!wasEmpty)
			o = std::stoi(data.GetTag("output").value);

		data.SetTag("output", std::to_string(output));

		if (!wasEmpty && o != 0)
		{
			outputCount++;
			data.SetTag("outputCount", std::to_string(outputCount));
		}
		else
		{
			data.SetTag("outputCount", "1");
		}

		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		if (gp->hud->inv->shown && gp->hud->inv->isFurnace)
		{
			gp->hud->inv->CreateInventory();
			gp->hud->inv->CreateFurnace();
			gp->hud->inv->UpdateInventory();
		}

		ticksNeeded = -1;
		ticks = -1;
		tickPerc = 0.0f;
		if (light)
		{
			LightingManager::GetInstance()->RemoveLight(position);
			currentChunk->modified = true;
		}
		light = false;
		data.SetTag("ticksLeft", "-1");
	}
	else
	{
		if (!light)
		{
			LightingManager::GetInstance()->AddLight(position, 10);
			currentChunk->modified = true;
		}
		light = true;
		data.SetTag("ticksLeft", std::to_string(ticks));
	}

	return true;
}

void Furnace::Destroy()
{
	if (light)
		LightingManager::GetInstance()->RemoveLight(position);
}
