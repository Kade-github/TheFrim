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

#include <vector>

namespace Data
{
	struct DataTag
	{
		char name[32];
		char value[64];

		MSGPACK_DEFINE_ARRAY(name, value);
	};

	struct BlockData
	{
		uint8_t x = 0, y = 0, z = 0;
		uint8_t type = 0;

		std::vector<DataTag> tags;

		MSGPACK_DEFINE_ARRAY(x, y, z, type, tags);

	};

	struct Chunk
	{
		bool isGenerated = false;
		BlockData blocks[CHUNK_SIZE][CHUNK_SIZE][CHUNK_HEIGHT];
		int32_t x, z;

		void placeBlock(int x, int y, int z, int type)
		{
			BlockData d;
			d.type = type;
			blocks[x][z][y] = d;
		}

		void removeBlock(int x, int y, int z)
		{
			blocks[x][z][y] = {};
		}

		MSGPACK_DEFINE_ARRAY(blocks, x, z);
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