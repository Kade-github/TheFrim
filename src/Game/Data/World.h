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

	struct BlockData
	{
		uint8_t x = 0, y = 0, z = 0;
		uint8_t type = 0;

		std::vector<DataTag> tags;

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
			for (DataTag t : tags)
			{
				if (t.name == name)
				{
					t.SetValue(value);
				}
			}
		}

		MSGPACK_DEFINE_ARRAY(x, y, z, type, tags);

	};

	struct DataChunk
	{
		std::vector<BlockData> blocks = {};

		MSGPACK_DEFINE_ARRAY(blocks);
	};

	struct Chunk
	{
		bool isGenerated = false;
		uint8_t blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_HEIGHT];
		int32_t x, z;
		DataChunk data;

		void placeBlock(int x, int y, int z, uint8_t block)
		{
			blocks[x][z][y] = block;
		}

		void removeBlock(int x, int y, int z)
		{
			blocks[x][z][y] = 0;
		}

		BlockData getBlockData(int x, int y, int z)
		{
			for (BlockData b : data.blocks)
			{
				if (b.x == x && b.y == y && b.z == z)
				{
					return b;
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
		

		MSGPACK_DEFINE_ARRAY(blocks, x, z, data);
	};

	struct Region {
		int32_t startX = 0, startZ = 0;
		int32_t endX = 0, endZ = 0;

		std::vector<std::vector<Chunk>> chunks = std::vector<std::vector<Chunk>>(REGION_SIZE);

		Region() {
			for (int i = 0; i < REGION_SIZE; i++)
				chunks[i] = std::vector<Chunk>(REGION_SIZE);
		};

		Chunk getChunk(int x, int z);
		Chunk* getChunkPtr(int x, int z);
		void addChunk(Chunk c);

		void freePlace(int x, int y, int z, int type);

		bool doesBlockExistInRange(int x, int y, int z, int type, int range);

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

		Player p;

		unsigned long seedNum = 0;

		std::string seed;

		World()
		{
			name = "";
		}

		void scanForRegions();

		void parseSeed();

		Region getRegion(int x, int z);

		Region generateRegion(int x, int z);

		void saveRegion(Region r);

		MSGPACK_DEFINE_ARRAY(name, seed, p);

	};
};
#endif