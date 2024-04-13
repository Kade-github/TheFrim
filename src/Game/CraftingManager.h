#ifndef _CRAFTINGMANAGER_H
#define _CRAFTINGMANAGER_H

#include <vector>
#include "Data/Player.h"

struct CraftingRecipe
{
	Data::InventoryItem result;
	Data::InventoryItem ingredients[3][3];

	bool anyWhere = false;

	bool IsMatch(Data::InventoryItem ingredients[3][3]);
};

class CraftingManager
{
public:
	static CraftingManager* GetInstance();

	std::vector<CraftingRecipe> Recipes;

	void Init();

	void AddRecipe(CraftingRecipe recipe);

	Data::InventoryItem Craft(Data::InventoryItem ingredients[3][3]);
};

#endif