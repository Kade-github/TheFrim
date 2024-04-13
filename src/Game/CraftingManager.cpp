#include "CraftingManager.h"

CraftingManager* CraftingManager::GetInstance()
{
	static CraftingManager instance;
	return &instance;
}

void CraftingManager::Init()
{
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
		if (recipe.anyWhere)
		{
			std::map<Data::ItemType, int> itemsRequired;

			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					if (ingredients[j][k].type != Data::ITEM_NULL)
					{
						itemsRequired[(Data::ItemType)ingredients[j][k].type] += ingredients[j][k].count;
					}
				}
			}

			// check if we have all the required items

			bool hasAllItems = true;

			for(int j = 0; j < 3; j++)
			{
				for(int k = 0; k < 3; k++)
				{
					if (itemsRequired.find((Data::ItemType)ingredients[j][k].type) == itemsRequired.end() || itemsRequired[(Data::ItemType)ingredients[j][k].type] < ingredients[j][k].count)
						hasAllItems = false;
				}

				if (!hasAllItems)
					break;
			}

			if (!hasAllItems)
				continue;
		}
		else if (!recipe.IsMatch(ingredients))
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
