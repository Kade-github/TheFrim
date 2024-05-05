#include "DroppedItem.h"
#include <Game.h>
#include "Chunk.h"
#include "../../WorldManager.h"

DroppedItem::DroppedItem(glm::vec3 _pos, Texture* t, Data::InventoryItem i) : Entity(_pos)
{
	sprite = new Sprite3D(t, _pos);
	sprite->width = 0.5f;
	sprite->height = 0.5f;
	sprite->depth = true;

	isCreature = false;

	blockMode = i.placeable;

	rotateAxis = glm::vec3(0, 1, 0);

	item = i;

	sprite->src = t->spriteSheet.GetUVFlip(item.tag);

	sprite->UpdateSprite();
	
	lifeTime = glfwGetTime();
}

DroppedItem::~DroppedItem()
{
	if (blockMode)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
	}
	delete sprite;
}

void DroppedItem::Draw()
{
	angle += 75.0f * Game::instance->deltaTime;

	if (angle > 360)
		angle = 0;

	sprite->position = position;
	sprite->rotateAxis = rotateAxis;
	sprite->angle = angle;

	Entity::Draw();

	sprite->Draw();

}
