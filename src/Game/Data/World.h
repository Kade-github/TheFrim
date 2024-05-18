#ifndef _WORLD_H
#define _WORLD_H

#define CHUNK_SIZE 16
#define CHUNK_HEIGHT 256

#define REGION_SIZE 5

#include "Player.h"

#include <vector>
#include <string>
#include <mutex>
#include <bitset>
#include <random>

namespace Data
{
	struct DataTag
	{
		char name[32] = "tag";
		char value[64] = "data";

		void Assemble(std::string n, std::string v)
		{
			strcpy(name, n.c_str());
			strcpy(value, v.c_str());
		}

		void SetName(std::string n)
		{
			strcpy(name, n.c_str());
		}

		void SetValue(std::string v)
		{
			strcpy(value, v.c_str());
		}

		bool IsReal()
		{
			return strcmp(name, "tag") != 0;
		}
		MSGPACK_DEFINE_ARRAY(name, value);
	};

	class BlockData
	{
	public:
		uint8_t x = 0, y = 0, z = 0;
		uint8_t type = 0;

		std::vector<DataTag> tags;

		~BlockData()
		{
			tags.clear();
		}

		DataTag GetTag(std::string name)
		{
			for(DataTag t : tags)
			{
				if (t.name == name)
					return t;
			}

			return DataTag();
		}

		void AddTag(std::string name, std::string value)
		{
			DataTag t;
			t.Assemble(name, value);
			tags.push_back(t);
		}

		void SetTag(std::string name, std::string value)
		{
			bool set = false;
			for (DataTag& t : tags)
			{
				if (t.name == name)
				{
					t.SetValue(value);
					set = true;
					break;
				}
			}

			if (!set)
			{
				AddTag(name, value);
			}
		}

		MSGPACK_DEFINE_ARRAY(x, y, z, type, tags);

	};

	class DataChunk
	{
	public:
		std::vector<BlockData> blocks = {};

		~DataChunk()
		{
			blocks.clear();
		}

		MSGPACK_DEFINE_ARRAY(blocks);
	};

	struct BlockChunk
	{
		uint8_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_HEIGHT];

		MSGPACK_DEFINE_ARRAY(blocks);
	};

	class Chunk
	{
	public:
		bool isGenerated = false;
		BlockChunk bChunk = {};
		int32_t x, z;
		DataChunk data = {};

		void placeBlock(int x, int y, int z, uint8_t block)
		{
			bChunk.blocks[x][z][y] = block;
		}

		void removeBlock(int x, int y, int z)
		{
			bChunk.blocks[x][z][y] = 0;
		}

		void setBlockData(int x, int y, int z, BlockData b)
		{
			for (int i = 0; i < data.blocks.size(); i++)
			{
				if (data.blocks[i].x == x && data.blocks[i].y == y && data.blocks[i].z == z)
				{
					data.blocks[i] = b;
					return;
				}
			}

			addBlockData(b, x, y, z);
		}

		BlockData getBlockData(int x, int y, int z)
		{
			for (int i = 0; i < data.blocks.size(); i++)
			{
				if (data.blocks[i].x == x && data.blocks[i].y == y && data.blocks[i].z == z)
				{
					return data.blocks[i];
				}
			}

			return BlockData();
		}

		void addBlockData(BlockData b, int x, int y, int z)
		{
			b.x = x;
			b.y = y;
			b.z = z;
			removeBlockData(b.x, b.y, b.z);
			data.blocks.push_back(b);
		}

		void removeBlockData(int x, int y, int z)
		{
			for (int i = 0; i < data.blocks.size(); i++)
			{
				if (data.blocks[i].x == x && data.blocks[i].y == y && data.blocks[i].z == z)
				{
					data.blocks.erase(data.blocks.begin() + i);
					return;
				}
			}
		}

		int getHighestBlock(int x, int z)
		{
			for (int i = CHUNK_HEIGHT - 1; i > 0; i--)
			{
				if (bChunk.blocks[x][z][i] != 0)
					return i;
			}

			return 0;
		}

		int amountOfBlockInRadius(int x, int y, int z, int type, int radius)
		{
			int amount = 0;

			int startY = y + radius;

			if (startY > CHUNK_HEIGHT)
				startY = CHUNK_HEIGHT;

			int endY = y - radius;

			if (endY < 0)
				endY = 0;

			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				for (int j = 0; j < CHUNK_SIZE; j++)
				{
					for (int k = startY; k > endY - 1; k--)
					{
						if (bChunk.blocks[i][j][k] == type)
						{
							if (abs(i - x) < radius && abs(j - z) < radius && abs(k - y) < radius)
								amount++;
						}
					}
				}
			}

			return amount;
		}

		void placeStone(int x, int y, int z, float multiplier = 1.0f);
		

		MSGPACK_DEFINE_ARRAY(bChunk, x, z, data);
	};

	struct Region {
		int32_t startX = 0, startZ = 0;
		int32_t endX = 0, endZ = 0;

		std::vector<std::vector<Chunk>> chunks = std::vector<std::vector<Chunk>>(REGION_SIZE);

		Region() {
			for (int i = 0; i < REGION_SIZE; i++)
				chunks[i] = std::vector<Chunk>(REGION_SIZE);
		};

		~Region()
		{
			for (int i = 0; i < REGION_SIZE; i++)
			{
				for (int j = 0; j < REGION_SIZE; j++)
				{
					chunks[i][j].~Chunk();
				}
			}
		}

		Chunk getChunk(int x, int z);
		Chunk* getChunkPtr(int x, int z);
		void addChunk(Chunk c);

		void freePlace(int x, int y, int z, int type);

		bool doesBlockExistInRange(int x, int y, int z, int type, int range);
		bool doesBlockExist(int x, int y, int z);

		Chunk generateChunk(int x, int z);

		void generateStructures();

		MSGPACK_DEFINE_ARRAY(startX, startZ, endX, endZ, chunks);
	};

	class World
	{
	public:
		std::string _path = "";
		std::vector<std::string> storedRegions;
		std::string name;

		float sunAngle = 90;
		int waterLevel = -1;

		int randomAdvancement = 0;

		Player p;

		unsigned long seedNum = 0;

		std::string seed;

		World()
		{
			name = "";
		}

		~World()
		{
			storedRegions.clear();
		}

		void scanForRegions();

		void parseSeed();

		Region getRegion(int x, int z);

		Region generateRegion(int x, int z);


		void saveRegion(Region r);

		MSGPACK_DEFINE_ARRAY(name, seed, p, waterLevel, sunAngle, randomAdvancement);

	};
};
#endif