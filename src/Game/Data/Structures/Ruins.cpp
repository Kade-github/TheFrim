#include "Ruins.h"
#include "../../Objects/Base/Block.h"

void Data::Ruins::Variant1()
{
}

void Data::Ruins::Variant2()
{
}

void Data::Ruins::Variant3()
{
}

void Data::Ruins::Variant4()
{
}

void Data::Ruins::Variant5()
{
}

void Data::Ruins::CreateVariations()
{
	Variant1();
	Variant2();
	Variant3();
	Variant4();
	Variant5();
}

void Data::Ruins::GenerateVariation()
{
	int r = rand() % variations.size();
	variation = r;
}

void Data::Ruins::Create(int x, int z, int y, Data::Chunk& c, Data::Region* r)
{
	if (r->doesBlockExistInRange(x, y, z, RUINED_COBBLESTONE, 42))
		return;

	Structure::Create(x, z, y, c, r);
}