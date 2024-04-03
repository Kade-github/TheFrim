#ifndef _PLAYER_DATA_H_
#define _PLAYER_DATA_H_

#include <string>
#include <vector>
#define MSGPACK_NO_BOOST
#include <msgpack.hpp>

#define PLAYER_MAX_HEALTH 10.0f

#define PLAYER_INVENTORY_WIDTH 9
#define PLAYER_INVENTORY_HEIGHT 4

namespace Data
{
	enum ItemType // so basically, block type is a uint8_t, max of 255. thus we can partition it into blocks and items.
	{
		ITEM_NULL = 0,

		// blocks (1-126)

		ITEM_DIRT = 1,
		ITEM_GRASS = 2,
		ITEM_STONE = 3,
		ITEM_COBBLESTONE = 4,
		ITEM_SAND = 5,
		ITEM_WOOD = 6,
		ITEM_WOODENPLANKS = 7,
		ITEM_LEAVES = 8,
		ITEM_CRAFTINGTABLE = 9,
		ITEM_FURNACE = 10,
		ITEM_CLAY = 11,
		ITEM_GLASS = 12,
		ITEM_IRON_ORE = 13,
		ITEM_GOLD_ORE = 14,
		ITEM_DIAMOND_ORE = 15,

		// items (127-255)

		ITEM_STICK = 127,
		ITEM_WOODEN_PICKAXE = 128,
		ITEM_WOODEN_SHOVEL = 129,
		ITEM_WOODEN_AXE = 130,
		ITEM_WOODEN_SWORD = 131,
		ITEM_STONE_PICKAXE = 132,
		ITEM_STONE_SHOVEL = 133,
		ITEM_STONE_AXE = 134,
		ITEM_STONE_SWORD = 135,
		ITEM_IRON_PICKAXE = 136,
		ITEM_IRON_SHOVEL = 137,
		ITEM_IRON_AXE = 138,
		ITEM_IRON_SWORD = 139,
		ITEM_GOLD_PICKAXE = 140,
		ITEM_GOLD_SHOVEL = 141,
		ITEM_GOLD_AXE = 142,
		ITEM_GOLD_SWORD = 143,
		ITEM_DIAMOND_PICKAXE = 144,
		ITEM_DIAMOND_SHOVEL = 145,
		ITEM_DIAMOND_AXE = 146,
		ITEM_DIAMOND_SWORD = 147,
		ITEM_APPLE = 148,
		ITEM_COAL = 149,
		ITEM_IRON_INGOT = 150,
		ITEM_GOLD_INGOT = 151,
		ITEM_DIAMOND = 152,
	};

	struct NBT {
		std::string key;
		std::string value;

		MSGPACK_DEFINE_ARRAY(key, value);
	};

	struct InventoryItem {
		int type = ITEM_NULL;
		int count = 1;

		std::vector<NBT> nbt = {};

		InventoryItem() {}

		InventoryItem(int type, int count)
			: type(type), count(count) {}

		NBT& GetNBT(std::string key)
		{
			static NBT n {"", ""};
			for (auto& n : nbt)
			{
				if (n.key == key)
					return n;
			}

			return n;
		}

		NBT FindNBT(std::string key)
		{
			for (auto& n : nbt)
			{
				if (n.key == key)
					return n;
			}

			return { "", "" };
		}

		MSGPACK_DEFINE_ARRAY(type, count, nbt);
	};

	struct Player
	{
		float x = 0, y = 180, z = 0;
		float pitch, yaw;

		uint8_t selectedSlot = 0;

		float health = PLAYER_MAX_HEALTH;

		InventoryItem inventory[PLAYER_INVENTORY_WIDTH][PLAYER_INVENTORY_HEIGHT] = {};

		InventoryItem& GetInventoryItem(int x, int y)
		{
			return inventory[x][y];
		}

		void SetInventoryItem(int x, int y, InventoryItem item)
		{
			inventory[x][y] = item;
		}

		void SetInventoryItem(int x, int y, int type, int count)
		{
			inventory[x][y] = { (int)type, count };
		}

		MSGPACK_DEFINE_ARRAY(x, y, z, pitch, yaw, selectedSlot, health, inventory);
	};
}

#endif