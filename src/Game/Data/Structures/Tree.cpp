#include "Tree.h"
#include "../../Objects/Base/Block.h"

using namespace Data;

void Tree::Create(int _x, int _z, int _y, Chunk& c, Region* r)
{
	int height = rand() % 5 + 6;

	if (height < 4)
		height = 4;

	if (height > 6)
		height = 6;

	int variationType = variation % 3;

	// check if it can be placed

	bool cantPlace = r->doesBlockExistInRange(c.x + _x, _y, c.z + _z, WOOD, 5);

	if (cantPlace)
		return;

	int offset = 1 * variationType;

	if (c.x + _x <= r->startX + 2)
		_x = 3;

	if (c.x + _x >= r->endX - 2)
		_x = CHUNK_SIZE - 3;

	if (c.z + _z <= r->startZ + 2)
		_z = 3;

	if (c.z + _z >= r->endZ - 2)
		_z = CHUNK_SIZE - 3;

	for (int i = 1; i < height + 1; i++)
	{
		c.placeBlock(_x, _y + i, _z, WOOD);
	}

	int _rx = c.x + _x;
	int _rz = c.z + _z;

	r->freePlace(_rx, _y + height, _rz, LEAVES);

	r->freePlace(_rx, _y + height, _rz + 1, LEAVES);
	r->freePlace(_rx, _y + height, _rz - 1, LEAVES);
	r->freePlace(_rx + 1, _y + height, _rz, LEAVES);
	r->freePlace(_rx - 1, _y + height, _rz, LEAVES);
	r->freePlace(_rx + 1, _y + height, _rz + 1, LEAVES);
	r->freePlace(_rx - 1, _y + height, _rz - 1, LEAVES);
	r->freePlace(_rx + 1, _y + height, _rz - 1, LEAVES);
	r->freePlace(_rx - 1, _y + height, _rz + 1, LEAVES);

	for (int g = 1; g < 3; g++)
	{
		for (int i = -2; i < 3; i++)
		{
			for (int j = -2; j < 3; j++)
			{
				if (i == 0 && j == 0)
					continue;

				r->freePlace(_rx + i, _y + height - g, _rz + j, LEAVES);
			}
		}
	}
}

void Tree::GenerateVariation()
{
	variation = rand() % 100;
}
