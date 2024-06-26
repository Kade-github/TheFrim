#ifndef _PLAYER_DATA_H_
#define _PLAYER_DATA_H_

#include <string>
#include <vector>

#define MSGPACK_NO_BOOST

#include <msgpack.hpp>

#define PLAYER_MAX_HEALTH 10.0f
#define PLAYER_MAX_AIR 10.0f

#define PLAYER_INVENTORY_WIDTH 9
#define PLAYER_INVENTORY_HEIGHT 4

namespace Data {
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
		// water would be here, ITEM_WATER = 13 (but it's not an item)
		ITEM_BEDROCK = 14,
		ITEM_COAL_ORE = 15,
		ITEM_IRON_ORE = 16,
		ITEM_GOLD_ORE = 17,
		ITEM_DIAMOND_ORE = 18,
		ITEM_TORCH = 19,
		ITEM_RUINED_COBBLESTONE = 20,
		ITEM_RUINED_DEBRIS = 21,
		ITEM_ROCKET = 22,
		ITEM_REINFORCED_IRON_BLOCK = 23,

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
		ITEM_IRON_HELMET = 153,
		ITEM_IRON_CHESTPLATE = 154,
		ITEM_IRON_BOOTS = 155,
		ITEM_GOLD_HELMET = 156,
		ITEM_GOLD_CHESTPLATE = 157,
		ITEM_GOLD_BOOTS = 158,
		ITEM_DIAMOND_HELMET = 159,
		ITEM_DIAMOND_CHESTPLATE = 160,
		ITEM_DIAMOND_BOOTS = 161,
		ITEM_ROCKET_ENGINE = 163,
		ITEM_CONDENSED_COAL = 164,
		ITEM_UNCOOKED_PORK = 165,
		ITEM_COOKED_PORK = 166,
		ITEM_KNOWLEDGEMENT_TABLET = 167,
	};

	struct NBT {
		std::string key;
		std::string value;

		MSGPACK_DEFINE_ARRAY(key, value);
	};

	struct InventoryItem {
		std::string tag = "item_null";

		bool stackable = true;
		bool equipable = false;
		bool placeable = false;
		bool edible = false;

		int armor = 0;
		int armorSlot = 0;

		int food = 0;

		float breakingPower = 1.0f;
		float durability = 1.0f;
		float damage = 0.5f;

		int type = ITEM_NULL;
		uint8_t count = 1;

		void SetTag() {
			stackable = true;
			equipable = false;
			placeable = false;

			if (count < 0)
				count = 1;

			armor = 0;
			armorSlot = 0;

			switch (type) {
			case ITEM_DIRT:
				tag = "item_dirt";
				placeable = true;
				break;
			case ITEM_GRASS:
				tag = "item_grass";
				placeable = true;
				break;
			case ITEM_STONE:
				tag = "item_stone";
				placeable = true;
				break;
			case ITEM_COBBLESTONE:
				tag = "item_cobblestone";
				placeable = true;
				break;
			case ITEM_SAND:
				tag = "item_sand";
				placeable = true;
				break;
			case ITEM_WOOD:
				tag = "item_wood";
				placeable = true;
				break;
			case ITEM_WOODENPLANKS:
				tag = "item_wooden_planks";
				placeable = true;
				break;
			case ITEM_LEAVES:
				tag = "item_leaves";
				placeable = true;
				break;
			case ITEM_CRAFTINGTABLE:
				tag = "item_crafting_table";
				placeable = true;
				break;
			case ITEM_FURNACE:
				tag = "item_furnace";
				placeable = true;
				break;
			case ITEM_CLAY:
				tag = "item_clay";
				placeable = true;
				break;
			case ITEM_GLASS:
				tag = "item_glass";
				placeable = true;
				break;
			case ITEM_BEDROCK:
				tag = "item_bedrock";
				placeable = true;
				break;
			case ITEM_COAL_ORE:
				tag = "item_coal_ore";
				placeable = true;
				break;
			case ITEM_IRON_ORE:
				tag = "item_iron_ore";
				placeable = true;
				break;
			case ITEM_GOLD_ORE:
				tag = "item_gold_ore";
				placeable = true;
				break;
			case ITEM_DIAMOND_ORE:
				tag = "item_diamond_ore";
				placeable = true;
				break;
			case ITEM_STICK:
				tag = "item_stick";
				break;
			case ITEM_WOODEN_PICKAXE:
				stackable = false;
				tag = "item_wood_pick";
				breakingPower = 2.0f;
				damage = 0.75f;
				break;
			case ITEM_WOODEN_SHOVEL:
				stackable = false;
				tag = "item_wood_shovel";
				breakingPower = 1.2f;
				damage = 0.75f;
				break;
			case ITEM_WOODEN_AXE:
				stackable = false;
				tag = "item_wood_axe";
				breakingPower = 1.2f;
				damage = 1.0;
				break;
			case ITEM_WOODEN_SWORD:
				stackable = false;
				tag = "item_wood_sword";
				damage = 1.25;
				break;
			case ITEM_STONE_PICKAXE:
				stackable = false;
				tag = "item_stone_pick";
				breakingPower = 4.0f;
				damage = 0.75f;
				break;
			case ITEM_STONE_SHOVEL:
				stackable = false;
				tag = "item_stone_shovel";
				breakingPower = 1.5f;
				damage = 0.75f;
				break;
			case ITEM_STONE_AXE:
				stackable = false;
				tag = "item_stone_axe";
				breakingPower = 1.5f;
				damage = 1.0f;
				break;
			case ITEM_STONE_SWORD:
				stackable = false;
				tag = "item_stone_sword";
				damage = 1.25f;
				break;
			case ITEM_IRON_PICKAXE:
				stackable = false;
				tag = "item_iron_pick";
				breakingPower = 6.0f;
				damage = 1.0f;
				break;
			case ITEM_IRON_SHOVEL:
				stackable = false;
				tag = "item_iron_shovel";
				breakingPower = 1.8f;
				damage = 1.0f;
				break;
			case ITEM_IRON_AXE:
				stackable = false;
				tag = "item_iron_axe";
				breakingPower = 1.8f;
				damage = 1.25f;
				break;
			case ITEM_IRON_SWORD:
				stackable = false;
				tag = "item_iron_sword";
				damage = 1.75f;
				break;
			case ITEM_GOLD_PICKAXE:
				stackable = false;
				tag = "item_gold_pick";
				breakingPower = 7.9f;
				damage = 1.25f;
				break;
			case ITEM_GOLD_SHOVEL:
				stackable = false;
				tag = "item_gold_shovel";
				breakingPower = 2.2f;
				damage = 1.25f;
				break;
			case ITEM_GOLD_AXE:
				stackable = false;
				tag = "item_gold_axe";
				breakingPower = 2.2f;
				damage = 1.5f;
				break;
			case ITEM_GOLD_SWORD:
				stackable = false;
				tag = "item_gold_sword";
				damage = 2.0f;
				break;
			case ITEM_DIAMOND_PICKAXE:
				stackable = false;
				tag = "item_diamond_pick";
				breakingPower = 8.0f;
				damage = 1.25f;
				break;
			case ITEM_DIAMOND_SHOVEL:
				stackable = false;
				tag = "item_diamond_shovel";
				breakingPower = 2.0f;
				damage = 1.25f;
				break;
			case ITEM_DIAMOND_AXE:
				stackable = false;
				tag = "item_diamond_axe";
				breakingPower = 2.0f;
				damage = 1.5f;
				break;
			case ITEM_DIAMOND_SWORD:
				stackable = false;
				tag = "item_diamond_sword";
				damage = 1.75f;
				break;
			case ITEM_APPLE:
				tag = "item_apple";
				food = 1;
				stackable = false;
				edible = true;
				break;
			case ITEM_COAL:
				tag = "item_coal";
				break;
			case ITEM_IRON_INGOT:
				tag = "item_iron_ingot";
				break;
			case ITEM_GOLD_INGOT:
				tag = "item_gold_ingot";
				break;
			case ITEM_DIAMOND:
				tag = "item_diamond";
				break;
			case ITEM_TORCH:
				tag = "item_torch";
				placeable = true;
				break;
			case ITEM_RUINED_COBBLESTONE:
				tag = "item_ruined_cobblestone";
				placeable = true;
				break;
			case ITEM_RUINED_DEBRIS:
				tag = "item_ruined_debris";
				placeable = true;
				break;
			case ITEM_ROCKET:
				tag = "item_rocket";
				placeable = true;
				break;
			case ITEM_REINFORCED_IRON_BLOCK:
				tag = "item_reinforced_iron_block";
				placeable = true;
				break;
			case ITEM_ROCKET_ENGINE:
				tag = "item_rocket_engine";
				break;
			case ITEM_CONDENSED_COAL:
				tag = "item_condensed_coal";
				break;
			case ITEM_IRON_HELMET:
				tag = "item_iron_helmet";
				armor = 2;
				equipable = true;
				break;
			case ITEM_IRON_CHESTPLATE:
				tag = "item_iron_chestplate";
				armor = 6;
				armorSlot = 1;
				equipable = true;
				break;
			case ITEM_IRON_BOOTS:
				tag = "item_iron_boots";
				armor = 2;
				armorSlot = 2;
				equipable = true;
				break;
			case ITEM_GOLD_HELMET:
				tag = "item_gold_helmet";
				armor = 3;
				equipable = true;
				break;
			case ITEM_GOLD_CHESTPLATE:
				tag = "item_gold_chestplate";
				armor = 8;
				armorSlot = 1;
				equipable = true;
				break;
			case ITEM_GOLD_BOOTS:
				tag = "item_gold_boots";
				armorSlot = 2;
				armor = 3;
				equipable = true;
				break;
			case ITEM_DIAMOND_HELMET:
				tag = "item_diamond_helmet";
				armor = 4;
				equipable = true;
				break;
			case ITEM_DIAMOND_CHESTPLATE:
				tag = "item_diamond_chestplate";
				armor = 10;
				armorSlot = 1;
				equipable = true;
				break;
			case ITEM_DIAMOND_BOOTS:
				tag = "item_diamond_boots";
				armorSlot = 2;
				armor = 4;
				equipable = true;
				break;
			case ITEM_UNCOOKED_PORK:
				tag = "item_uncooked_pork";
				food = 2;
				stackable = false;
				edible = true;
				break;
			case ITEM_COOKED_PORK:
				tag = "item_cooked_pork";
				food = 4;
				stackable = false;
				edible = true;
				break;
			case ITEM_KNOWLEDGEMENT_TABLET:
				tag = "item_knowledge_tablet";
				stackable = false;
				break;
			default:
				tag = "item_null";
				damage = 0;
				break;
			}
		}

		std::vector<NBT> nbt = {};

		InventoryItem() {}

		InventoryItem(int type, int count)
			: type(type), count(count) {
			SetTag();
		}

		void SetNBT(std::string key, std::string value) {
			for (auto& n : nbt) {
				if (n.key == key) {
					n.value = value;
					return;
				}
			}

			nbt.push_back({ key, value });
		}

		NBT& GetNBT(std::string key) {
			static NBT n{ "", "" };
			for (auto& n : nbt) {
				if (n.key == key)
					return n;
			}
			n.value = "";
			n.key = "";
			return n;
		}

		NBT FindNBT(std::string key) {
			for (auto& n : nbt) {
				if (n.key == key)
					return n;
			}

			return { "", "" };
		}

		MSGPACK_DEFINE_ARRAY(type, count, nbt, tag, durability, placeable, equipable, stackable, armor, armorSlot,
			damage, breakingPower, food, edible);
	};

	struct Player {
		float x = 0.5, y = -100, z = 0.5;
		float pitch = 0, yaw = 0;

		uint8_t selectedSlot = 0;

		float health = PLAYER_MAX_HEALTH;
		float air = PLAYER_MAX_AIR;

		InventoryItem inventory[PLAYER_INVENTORY_WIDTH][PLAYER_INVENTORY_HEIGHT] = {};

		InventoryItem armor[3] = {};

		InventoryItem& GetInventoryItem(int x, int y) {
			return inventory[x][y];
		}

		void SetInventoryItem(int x, int y, InventoryItem item) {
			inventory[x][y] = item;
		}

		void SetInventoryItem(int x, int y, int type, int count) {
			inventory[x][y] = { (int)type, count };
		}

		bool GiveItem(InventoryItem item) {
			// find if it's in the inventory
			if (item.stackable)
				for (int y = 0; y < PLAYER_INVENTORY_HEIGHT; y++) {
					for (int x = 0; x < PLAYER_INVENTORY_WIDTH; x++) {
						InventoryItem& i = inventory[x][y];

						if (i.type == ITEM_NULL)
							continue;

						if (i.type == item.type) {
							if (i.count + item.count < 64) {
								i.count += item.count;
								return true;
							}
							else {
								float diff = i.count + item.count - 64;

								i.count = 64;
								item.count = diff;
							}
						}
					}
				}

			// find first empty slot

			for (int y = PLAYER_INVENTORY_HEIGHT - 1; y >= 0; y--) {
				for (int x = 0; x < PLAYER_INVENTORY_WIDTH; x++) {
					InventoryItem& i = inventory[x][y];

					if (i.type == ITEM_NULL) {
						i = item;
						return true;
					}
				}
			}

			return false;
		}

		MSGPACK_DEFINE_ARRAY(x, y, z, pitch, yaw, selectedSlot, health, inventory, armor);
	};
}

#endif