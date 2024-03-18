#ifndef _STRUCTURE_TREE_H
#define _STRUCTURE_TREE_H

#include "../Structure.h"

namespace Data
{
	class Tree : public Structure {
	public:
		void Create(int x, int z, int y, Chunk& c, Region* r) override;
		void GenerateVariation() override;
	};
}

#endif