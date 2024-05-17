#ifndef _STRUCTURE_RUINS_H
#define _STRUCTURE_RUINS_H

#include "../Structure.h"

namespace Data
{
	class Ruins : public Structure {
	public:
		Ruins()
		{
			CreateVariations();
		}
		void Variant1();
		void Variant2();
		void Variant3();
		void Variant4();
		void Variant5();

		void CreateVariations() override;
		void GenerateVariation() override;

		void Create(int x, int z, int y, Data::Chunk& c, Data::Region* r) override;
	};
}

#endif