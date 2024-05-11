#include "AI.h"
#include "../../WorldManager.h"

glm::vec3 AI::GenerateRandomPosition(float distance)
{
	// get a random position within a distance from the current position

	// get a random angle

	float angle = (rand() % 360) * 3.14159 / 180;

	// get a random distance

	float dist = (rand() % (int)distance) + 1;

	// calculate the new position

	glm::vec3 newPos = glm::vec3(position.x + cos(angle) * dist, position.y, position.z + sin(angle) * dist);

	Chunk* c = WorldManager::instance->GetChunk(newPos.x, newPos.z);

	if (c == nullptr)
		return glm::vec3();

	// get the lowest block from our position

	for (int y = (int)newPos.y; y > -1; y--)
	{
		if (c->GetBlock(newPos.x, y, newPos.z) > 0)
		{
			newPos.y = y + 1;
			break;
		}
	}

	return newPos;
}

void AI::MoveTo(glm::vec3 pos)
{
	target = pos;

	// get the path to the target


}

void AI::MoveToRandom()
{
	glm::vec3 r = GenerateRandomPosition(10);
	if (std::abs(r.y - position.y) > 4)
		return;

	MoveTo(r);
}

void AI::Draw()
{
	// move to target if we have one

	float distance = glm::distance(position, target);


	Entity::Draw(); // physics
}
