#ifndef _STRUCTURE_H
#define _STRUCTURE_H

#include "World.h"

enum StructureType {
	Tree,
};

class Structure {
public:
	int variation = 0;

	virtual void GenerateVariation() {
		variation = rand() % 100;
	};

	virtual void Create(int x, int z, int y, Data::Chunk& c, Data::Region* r) {};
};

#endif