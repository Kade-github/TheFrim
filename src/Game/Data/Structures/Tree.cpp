#include "Tree.h"
#include "../../Objects/Base/Block.h"

using namespace Data;

void Data::Tree::Variant1()
{
	Blueprint v;

	v.negXWidth = 3;
	v.xWidth = 3;
	v.negZWidth = 3;
	v.zWidth = 3;

	v.blocks.push_back(BlueprintBlock(LEAVES, 0, 6, 0));
	for (int y = 5; y > -1; y--) // stem
		v.blocks.push_back(BlueprintBlock(WOOD, 0, y, 0));

	// 3x3 at top
	for (int x = -1; x < 2; x++)
	{
		for (int z = -1; z < 2; z++)
		{
			if (x == 0 && z == 0) // dont replace stem
				continue;

			v.blocks.push_back(BlueprintBlock(LEAVES, x, 5, z));
		}
	}

	// 4x4 from 4-2
	for (int x = -2; x < 3; x++)
	{
		for (int z = -2; z < 3; z++)
		{
			for (int y = 4; y > 2; y--)
			{
				if (x == 0 && z == 0) // dont replace stem
					continue;

				v.blocks.push_back(BlueprintBlock(LEAVES, x, y, z));
			}
		}
	}

	variations.push_back(v);
}

void Data::Tree::Variant2()
{
	Blueprint v;

	v.negXWidth = 2;
	v.xWidth = 2;
	v.negZWidth = 2;
	v.zWidth = 2;

	// 3x3 at tippy top
	for (int x = -1; x < 2; x++)
	{
		for (int z = -1; z < 2; z++)
		{
			v.blocks.push_back(BlueprintBlock(LEAVES, x, 6, z));
		}
	}

	for (int y = 5; y > -1; y--) // stem
		v.blocks.push_back(BlueprintBlock(WOOD, 0, y, 0));

	// 4x4 at top
	for (int x = -2; x < 3; x++)
	{
		for (int z = -2; z < 3; z++)
		{
			if (x == 0 && z == 0) // dont replace stem
				continue;

			v.blocks.push_back(BlueprintBlock(LEAVES, x, 5, z));
			v.blocks.push_back(BlueprintBlock(LEAVES, x, 4, z));
		}
	}

	// 4x4 from 4-2
	for (int x = -2; x < 3; x++)
	{
		for (int z = -2; z < 3; z++)
		{
			for (int y = 3; y > 2; y--)
			{
				if (x == 0 && z == 0) // dont replace stem
					continue;

				v.blocks.push_back(BlueprintBlock(LEAVES, x, y, z));
			}
		}
	}

	variations.push_back(v);
}

void Data::Tree::Variant3()
{
	Blueprint v;

	v.negXWidth = 3;
	v.xWidth = 3;
	v.negZWidth = 3;
	v.zWidth = 3;

	v.blocks.push_back(BlueprintBlock(LEAVES, 0, 10, 0));
	for (int y = 9; y > -1; y--) // stem
		v.blocks.push_back(BlueprintBlock(WOOD, 0, y, 0));

	// 3x3 at top
	for (int x = -1; x < 2; x++)
	{
		for (int z = -1; z < 2; z++)
		{
			if (x == 0 && z == 0) // dont replace stem
				continue;

			v.blocks.push_back(BlueprintBlock(LEAVES, x, 9, z));
			v.blocks.push_back(BlueprintBlock(LEAVES, x, 8, z));
		}
	}

	// 4x4 from 8-6
	for (int x = -2; x < 3; x++)
	{
		for (int z = -2; z < 3; z++)
		{
			for (int y = 7; y > 4; y--)
			{
				if (x == 0 && z == 0) // dont replace stem
					continue;

				v.blocks.push_back(BlueprintBlock(LEAVES, x, y, z));
			}
		}
	}

	variations.push_back(v);
}


void Tree::CreateVariations()
{
	Variant1();
	Variant2();
	Variant3();
}

void Data::Tree::GenerateVariation()
{
	variation = rand() % 100;

	// 20% chance of a big tree
	if (variation < 20)
		variation = 2;
	else if (variation < 50 && variation >= 20)
		variation = 1;
	else
		variation = 0;

}

void Data::Tree::Create(int x, int z, int y, Data::Chunk& c, Data::Region* r)
{
	if (r->doesBlockExistInRange(x, y, z, WOOD, 10))
		return;

	Structure::Create(x, z, y, c, r);
}