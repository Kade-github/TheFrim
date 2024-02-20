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
		int32_t x, y, z;

		MSGPACK_DEFINE_ARRAY(blocks, x, y, z);
	};

	class World
	{
	public:

		std::string name;
		std::vector<Chunk> chunks;

		World()
		{
			name = "";
			chunks = {};
		}

		Chunk generateChunk(int x, int z);

		MSGPACK_DEFINE_ARRAY(name, chunks);

	};
};
#endif