#include "CraftingManager.h"
#include <Game.h>

CraftingManager* CraftingManager::GetInstance() {
	static CraftingManager instance;
	return &instance;
}

void CraftingManager::Init() {
	Recipes.clear();

#pragma region Recipes

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_WOODENPLANKS, 4),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_WOOD, 1)}, {}, {}
							  },
							  {
									  {}, {}, {}
							  },
							  {
									  {}, {}, {}
							  }
					  },
		});

	Recipes.back().anywhere = true;


	AddRecipe({
					  Data::InventoryItem(Data::ITEM_STICK, 4),
                      {
							  {
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}, {}
							  },
							  {
									  {}, {}, {}
							  }
					  },
		});

	// add a substitute recipe for sticks

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_STICK, 4),
										  {
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
												  },
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
												  },
												  {
														  {}, {}, {}
												  }
										  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_STICK, 4),
										  {
												  {
														  {}, {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  },
												  {
														  {}, {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  },
												  {
														  {}, {}, {}
												  }
										  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_STICK, 4),
										  {
												  {
														  {}, {}, {}
												  },
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
												  },
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
												  }
										  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_STICK, 4),
										  {
												  {
														  {}, {}, {}
												  },
												  {
														  {}, {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  },
												  {
														  {}, {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  }
										  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_STICK, 4),
										  {
												  {
														  {}, {}, {}
												  },
												  {
														  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}, {}
												  },
												  {
														  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}, {}
												  }
										  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_CRAFTINGTABLE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
							  },
							  {
									  {}, {}, {}
							  }
					  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_CRAFTINGTABLE, 1),
										 {
												  {
														  {}, {}, {}
												  },
												  {
														  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
												  },
												  {
														  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
												  }
										  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_CRAFTINGTABLE, 1),
										 {
												  {
														  {}, {}, {}
												  },
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  },
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  }
										  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_CRAFTINGTABLE, 1),
										  {
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  },
												  {
														  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
												  },
												  {
														  {},{}, {}
												  }
										  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_WOODEN_AXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},
									  {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_WOODEN_PICKAXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)},
									  {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_WOODEN_SWORD, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_WOODEN_SHOVEL, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_WOODENPLANKS, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_STONE_AXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_STONE_PICKAXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_STONE_SWORD, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_STONE_SHOVEL, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_IRON_AXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_IRON_PICKAXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_IRON_SWORD, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_IRON_SHOVEL, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_GOLD_AXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_GOLD_PICKAXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_GOLD_SWORD, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_GOLD_SHOVEL, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_DIAMOND_AXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_DIAMOND_PICKAXE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_DIAMOND_SWORD, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_DIAMOND_SHOVEL, 1),
					  {
							  {
									  {}, {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  },
							  {
									  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_TORCH, 4),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_COAL, 1)}, {}, {}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}, {}
							  },
							  {
									  {}, {}, {}
							  }
					  },
		});

	Recipes.back().AddSubstitute({
										  Data::InventoryItem(Data::ITEM_TORCH, 4),
										  {
												  {
														  {}, {Data::InventoryItem(Data::ITEM_COAL, 1)}, {}
												  },
												  {
														  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
												  },
												  {
														  {}, {}, {}
												  }
										  },
		});

	Recipes.back().AddSubstitute({
									  Data::InventoryItem(Data::ITEM_TORCH, 4),
									  {
											  {
													  {}, {}, {Data::InventoryItem(Data::ITEM_COAL, 1)}
											  },
											  {
													  {}, {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}
											  },
											  {
													  {}, {}, {}
											  }
									  },
		});

	Recipes.back().AddSubstitute({
									  Data::InventoryItem(Data::ITEM_TORCH, 4),
									  {
											  {
													  {}, {}, {}
											  },
											  {
													  {}, {Data::InventoryItem(Data::ITEM_COAL, 1)}, {}
											  },
											  {
													  {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}
											  }
									  },
		});

	Recipes.back().AddSubstitute({
									  Data::InventoryItem(Data::ITEM_TORCH, 4),
									  {
											  {
													  {}, {}, {}
											  },
											  {
													  {}, {}, {Data::InventoryItem(Data::ITEM_COAL, 1)}
											  },
											  {
													  {}, {}, {Data::InventoryItem(Data::ITEM_STICK, 1)}
											  }
									  },
		});

	Recipes.back().AddSubstitute({
									  Data::InventoryItem(Data::ITEM_TORCH, 4),
									  {
											  {
													  {}, {}, {}
											  },
											  {
													  {Data::InventoryItem(Data::ITEM_COAL, 1)}, {}, {}
											  },
											  {
													  {Data::InventoryItem(Data::ITEM_STICK, 1)}, {}, {}
											  }
									  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_FURNACE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}, {},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)},
									  {Data::InventoryItem(Data::ITEM_COBBLESTONE, 1)}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_IRON_HELMET, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  },
							  {
									  {}, {}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_IRON_CHESTPLATE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_IRON_BOOTS, 1),
					  {
							  {
									  {},
									  {},
									  {},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_GOLD_HELMET, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  },
							  {
									  {}, {}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_GOLD_CHESTPLATE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_GOLD_BOOTS, 1),
					  {
							  {
									  {},
									  {},
									  {},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}, {},
									  {Data::InventoryItem(Data::ITEM_GOLD_INGOT, 1)}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_DIAMOND_HELMET, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  },
							  {
									  {}, {}, {}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_DIAMOND_CHESTPLATE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_DIAMOND_BOOTS, 1),
					  {
							  {
									  {},
									  {},
									  {},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}, {},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)}
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 2),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_DIAMOND, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_CONDENSED_COAL, 4),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
									  {Data::InventoryItem(Data::ITEM_COAL, 1)},
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_ROCKET_ENGINE, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
									  {Data::InventoryItem(Data::ITEM_CONDENSED_COAL, 1)},
									  {Data::InventoryItem(Data::ITEM_IRON_INGOT, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
							  }
					  },
		});

	AddRecipe({
					  Data::InventoryItem(Data::ITEM_ROCKET, 1),
					  {
							  {
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
									  {Data::InventoryItem(Data::ITEM_ROCKET_ENGINE, 1)},
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
							  },
							  {
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
									  {Data::InventoryItem(Data::ITEM_REINFORCED_IRON_BLOCK, 1)},
							  }
					  },
		});

#pragma endregion

}

void CraftingManager::AddRecipe(CraftingRecipe recipe) {
	Recipes.push_back(recipe);
}

Data::InventoryItem CraftingManager::Craft(Data::InventoryItem ingredients[3][3]) {
	for (int i = 0; i < Recipes.size(); i++) {
		CraftingRecipe recipe = Recipes[i];

		bool match = false;

		if (recipe.substitutes.size() > 0) {
			for (int j = 0; j < recipe.substitutes.size(); j++) {
				if (recipe.substitutes[j].IsMatch(ingredients))
					match = true;
			}
		}

		if (recipe.anywhere)
		{
			match = true;

			// how many ingredients do we need
			std::map<int, int> needed;

			// how many ingredients do we have
			std::map<int, int> have;

			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					needed[recipe.ingredients[j][k].type] += recipe.ingredients[j][k].count;
				}
			}

			// how many ingredients do we have

			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					have[ingredients[j][k].type] += ingredients[j][k].count;
				}
			}

			for (auto& [key, value] : needed) {
				if (have[key] < value)
				{
					match = false;
					break;
				}
			}

		}

		if (!recipe.IsMatch(ingredients) && !match)
			continue;

		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				ingredients[j][k].count -= Recipes[i].ingredients[j][k].count;

				if (ingredients[j][k].count <= 0)
					ingredients[j][k] = {};
			}
		}
		return Recipes[i].result;
	}
	return {};

}

bool CraftingRecipe::IsMatch(Data::InventoryItem ingredients[3][3]) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			if (ingredients[i][j].type != this->ingredients[i][j].type ||
				ingredients[i][j].count < this->ingredients[i][j].count)
				return false;
		}
	}
	return true;
}

void CraftingRecipe::AddSubstitute(CraftingRecipe recipe)
{
	substitutes.push_back(recipe);
}
