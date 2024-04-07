#include "DroppedItem.h"
#include <Game.h>

DroppedItem::DroppedItem(glm::vec3 _pos, Texture* t, Data::InventoryItem i) : Entity(_pos)
{
	sprite = new Sprite3D(t, _pos);
	sprite->width = 0.5f;
	sprite->height = 0.5f;

	isCreature = false;

	rotateAxis = glm::vec3(0, 1, 0);

	item = i;

	sprite->src = t->spriteSheet.GetUVFlip(item.tag);

	sprite->UpdateSprite();

	lifeTime = glfwGetTime();
}

void DroppedItem::Draw()
{
	angle += 75.0f * Game::instance->deltaTime;

	if (angle > 360)
		angle = 0;

	sprite->position = position;
	sprite->rotateAxis = rotateAxis;
	sprite->angle = angle;

	sprite->Draw();

	Entity::Draw();
}