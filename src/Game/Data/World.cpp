#include "World.h"

Data::Chunk Data::World::generateChunk(int x, int z)
{
	Chunk chunk;

	chunk.x = x;
	chunk.y = 0;
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