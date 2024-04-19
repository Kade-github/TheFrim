#include "Water.h"

#include "../Chunk.h"
#include "../../../WorldManager.h"

std::vector<glm::vec3> Water::GetFreeSpaces(glm::vec3 _pos)
{
	std::vector<glm::vec3> freeSpaces = {};

	// Get our current chunk

	Chunk* c = WorldManager::instance->GetChunk(_pos.x, _pos.z);

	if (c == nullptr)
		return {};

	if (!c->DoesBlockExist(_pos.x, _pos.y - 1, _pos.z))
		return {};

	if (!c->DoesBlockExist(_pos.x, _pos.y, _pos.z - 1))
		freeSpaces.push_back(glm::vec3(_pos.x, _pos.y, _pos.z - 1));

	if (!c->DoesBlockExist(_pos.x, _pos.y, _pos.z + 1))
		freeSpaces.push_back(glm::vec3(_pos.x, _pos.y, _pos.z + 1));

	if (!c->DoesBlockExist(_pos.x - 1, _pos.y, _pos.z))
		freeSpaces.push_back(glm::vec3(_pos.x - 1, _pos.y, _pos.z));

	if (!c->DoesBlockExist(_pos.x + 1, _pos.y, _pos.z))
		freeSpaces.push_back(glm::vec3(_pos.x + 1, _pos.y, _pos.z));

	return freeSpaces;
}

void Water::PlaceWater(glm::vec3 _pos, int _strength)
{
	if (_strength <= 0)
		return;

	Chunk* c = WorldManager::instance->GetChunk(_pos.x, _pos.z);

	_pos -= c->position;

	Water* b = (Water*)c->CreateBlock(_pos.x, _pos.y, _pos.z, WATER);

	b->source = false;
	b->strength = _strength;

	c->PlaceBlock(_pos.x, _pos.y, _pos.z, b);

	changedBlocks = true;
}

void Water::Update() // water functionality
{
	if (strength == 0)
		return;

	std::vector<glm::vec3> freeSpaces = GetFreeSpaces(position);

	if (freeSpaces.size() == 0)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z);

		if (c == nullptr)
			return;

		// go down
		if (c->DoesBlockExist(position.x, position.y - 1, position.z))
			return;

		PlaceWater(glm::vec3(position.x, position.y - 1, position.z), strength - 1);

		return;
	}

	for (glm::vec3 freeSpace : freeSpaces)
	{
		Chunk* c = WorldManager::instance->GetChunk(freeSpace.x, freeSpace.z);

		PlaceWater(freeSpace, strength - 1);
	}

}