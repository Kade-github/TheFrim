#include "Tree.h"
#include "../../Objects/Base/Block.h"

using namespace Data;

void Tree::CreateVariations()
{
	Blueprint v1;

	v1.negXWidth = 3;
	v1.xWidth = 3;
	v1.negZWidth = 3;
	v1.zWidth = 3;

	v1.blocks.push_back(BlueprintBlock(LEAVES, 0, 6, 0));
	for(int y = 5; y > -1; y--) // stem
		v1.blocks.push_back(BlueprintBlock(WOOD, 0, y, 0));

	// 3x3 at top
	for(int x = -1; x < 2; x++)
	{
		for(int z = -1; z < 2; z++)
		{
			if (x == 0 && z == 0) // dont replace stem
				continue;
			
			v1.blocks.push_back(BlueprintBlock(LEAVES, x, 5, z));
		}
	}

	// 4x4 from 4-2
	for(int x = -2; x < 3; x++)
	{
		for (int z = -2; z < 3; z++)
		{
			for(int y = 4; y > 1; y++)
			{
				v1.blocks.push_back(BlueprintBlock(LEAVES, x, y, z));
			}
		}
	}

	variations.push_back(v1);
}