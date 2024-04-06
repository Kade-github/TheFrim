#include "Inventory.h"

Inventory::Inventory(glm::vec3 _pos, Player* _player) : BoxUI(_pos, 5, 10)
{
	player = _player;
}

void Inventory::MouseClick(int button, glm::vec2 pos)
{
}

void Inventory::MouseRelease(int button, glm::vec2 pos)
{
}

void Inventory::Draw()
{
	if (!shown)
		return;


	BoxUI::Draw();
}
