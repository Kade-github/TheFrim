#ifndef _STRUCTURE_TREE_H
#define _STRUCTURE_TREE_H

#include "../Structure.h"

namespace Data
{
	class Tree : public Structure {
	public:
		Tree()
		{
			CreateVariations();
		}
		void Variant1();
		void Variant2();
		void Variant3();

		void CreateVariations() override;
		void GenerateVariation() override;

		void Create(int x, int z, int y, Data::Chunk& c, Data::Region* r) override;
	};
}

#endif