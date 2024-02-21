#ifndef _WORLD_H
#define _WORLD_H

#define MSGPACK_NO_BOOST
#include <msgpack.hpp>
#include <vector>
#include <string>
#include <mutex>

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
		int32_t startX, startZ;
		int32_t endX, endZ;

		std::vector<Chunk> chunks;

		Chunk getChunk(int x, int z);
		void addChunk(Chunk c);

		inline Chunk generateChunk(int x, int z)
		{
			Chunk chunk;

			chunk.x = x;
			chunk.z = z;


			for (int _x = 0; _x < 16; _x++)
			{
				for (int _z = 0; _z < 16; _z++)
				{
					for (int _y = 128; _y < 256; _y++)
					{
						if (_y == 128)
							chunk.blocks[_y][_x][_z] = 2;
						else
							chunk.blocks[_y][_x][_z] = 1;
					}
				}
			}

			return chunk;
		}

		MSGPACK_DEFINE_ARRAY(startX, startZ, endX, endZ, chunks);
	};

	class World
	{
	public:
		std::string _path = "";
		std::string name;
		std::vector<std::string> regions;

		World()
		{
			name = "";
		}

		Region getRegion(int x, int z);

		MSGPACK_DEFINE_ARRAY(name, regions);

	};
};
#endif