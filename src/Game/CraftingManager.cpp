#include "CraftingManager.h"
#include <Game.h>

CraftingManager* CraftingManager::GetInstance()
{
	static CraftingManager instance;
	return &instance;
}

void CraftingManager::Init()
{
	Recipes.clear();

	#pragma region Recipes

	AddRecipe({
		Data::InventoryItem(Data::ITEM_WOODENPLANKS, 4),
		{
			{
				{Data::InventoryItem(Data::ITEM_WOOD, 1)},{},{}
			},
			{
				{},{},{}
			},
			{
				{},{},{}
			}
		},
		true
	});


	AddRecipe({
		Data::InventoryItem(Data::ITEM_STICK, 4),
		{
			{
				{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 2)},{},{}
			},
			{
				{},{},{}
			},
			{
				{},{},{}
			}
		},
		true
	});

	AddRecipe({
		Data::InventoryItem(Data::ITEM_CRAFTINGTABLE, 1),
		{
			{
				{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{}
			},
			{
				{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{}
			},
			{
				{},{},{}
			}
		},
		true
	});

	AddRecipe({
		Data::InventoryItem(Data::ITEM_WOODEN_AXE, 1),
		{
			{
				{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{}
			},
			{
				{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{Data::InventoryItem(Data::ITEM_STICK, 1)},{}
			},
			{
				{},	{Data::InventoryItem(Data::ITEM_STICK, 1)},{}
			}
		},
		true
	});

	AddRecipe({
		Data::InventoryItem(Data::ITEM_WOODEN_PICKAXE, 1),
		{
			{
				{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
			},
			{
				{},{Data::InventoryItem(Data::ITEM_STICK, 1)},{}
			},
			{
				{},{Data::InventoryItem(Data::ITEM_STICK, 1)},{}
			}
		},
		true
	});

	AddRecipe({
		Data::InventoryItem(Data::ITEM_WOODEN_SWORD, 1),
		{
			{
				{},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{}
			},
			{
				{},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{}
			},
			{
				{},{Data::InventoryItem(Data::ITEM_STICK, 1)},{}
			}
		},
		true
	});

	AddRecipe({
		Data::InventoryItem(Data::ITEM_WOODEN_SHOVEL, 1),
		{
			{
				{},{Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},{}
			},
			{
				{},{Data::InventoryItem(Data::ITEM_STICK, 1)},{}
			},
			{
				{},{Data::InventoryItem(Data::ITEM_STICK, 1)},{}
			}
		},
		true
	});

	#pragma endregion

}

void CraftingManager::AddRecipe(CraftingRecipe recipe)
{
	Recipes.push_back(recipe);
}

Data::InventoryItem CraftingManager::Craft(Data::InventoryItem ingredients[3][3])
{
	for(int i = 0; i < Recipes.size(); i++)
	{
		CraftingRecipe recipe = Recipes[i];
		if (!recipe.IsMatch(ingredients))
			continue;

		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				ingredients[j][k].count -= Recipes[i].ingredients[j][k].count;

				if (ingredients[j][k].count <= 0)
					ingredients[j][k] = {};
			}
		}
		return Recipes[i].result;
	}
	return Data::InventoryItem();	
	
}

bool CraftingRecipe::IsMatch(Data::InventoryItem ingredients[3][3])
{
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			if(ingredients[i][j].type != this->ingredients[i][j].type || ingredients[i][j].count < this->ingredients[i][j].count)
				return false;
		}
	}
	return true;
}
