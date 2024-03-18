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
		void CreateVariations() override;
	};
}

#endif