#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "World.h"

enum StructureType {
	Tree,
};

struct BlueprintBlock {
	uint8_t type = 0;

	int x = 0, y = 0, z = 0;
};

struct Blueprint {
	std::vector<BlueprintBlock> blocks = {};

	int negXWidth = 0;
	int negZWidth = 0;
	int xWidth = 0;
	int zWidth = 0;

};

class Structure {
public:
	int variation = 0;

	std::vector<Blueprint> variations;

	virtual void GenerateVariation() {
		variation = rand() % 100;
		if (variation <= 0)
			variation = 0;
	};
	
	virtual void CreateVariations() {};

	virtual void Create(int x, int z, int y, Data::Chunk& c, Data::Region* r) {
		GenerateVariation();

		if (variations.size() == 0)
			return;

		if (variation > variations.size() - 1)
			variation = variations.size() - 1;

		Blueprint& b = variations[variation];

		// size it inside the region
		
		int _x = x;
		int _z = z;

		if (_x - b.negXWidth < r->startX)
		{
			_x += b.negXWidth;

			if (_x > r->endX) // cant fit
				return;
		}

		if (_x + b.xWidth > r->endX)
		{
			_x -= b.xWidth;

			if (_x < r->startX) // cant fit
				return;
		}

		if (_z - b.negZWidth < r->startZ)
		{
			_z += b.negZWidth;

			if (_x > r->endZ) // cant fit
				return;
		}

		if (_z + b.zWidth > r->endZ)
		{
			_z -= b.zWidth;

			if (_x < r->startZ) // cant fit
				return;
		}

		for(BlueprintBlock& block : b.blocks)
			r->freePlace(_x + block.x, y + block.y, _z + block.z, block.type);
	};
};

#endif