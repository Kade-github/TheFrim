#ifndef _WORLD_H
#define _WORLD_H

#define MSGPACK_NO_BOOST
#include <msgpack.hpp>
#include <vector>
#include <string>
#include <mutex>
#include <bitset>

namespace Data
{

	struct Chunk
	{
		bool isGenerated = false;
		uint8_t blocks[256][16][16];
		int32_t x, z;

		MSGPACK_DEFINE_ARRAY(blocks, x, z);
	};

	struct Region {
		int32_t startX = 0, startZ = 0;
		int32_t endX = 0, endZ = 0;

		std::vector<std::vector<Chunk>> chunks = std::vector<std::vector<Chunk>>(5);

		Region() {
			for (int i = 0; i < 5; i++)
				chunks[i] = std::vector<Chunk>(5);
		};

		Chunk getChunk(int x, int z);
		void addChunk(Chunk c);

		Chunk generateChunk(int x, int z);

		MSGPACK_DEFINE_ARRAY(startX, startZ, endX, endZ, chunks);
	};

	class World
	{
	public:
		std::string _path = "";
		std::vector<std::string> storedRegions;
		std::string name;

		uint16_t seed;

		World()
		{
			name = "";
		}

		void scanForRegions();

		Region getRegion(int x, int z, int endX, int endZ);

		Region generateRegion(int x, int z);

		void saveRegion(Region r);

		MSGPACK_DEFINE_ARRAY(name);

	};
};
#endif