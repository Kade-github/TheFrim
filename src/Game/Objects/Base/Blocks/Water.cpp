#include "Water.h"

#include "../Chunk.h"
#include "../../../WorldManager.h"

Water::Water(glm::vec3 _position) : Block(_position, BlockType::WATER) {
	position = _position;

	soundType = SoundType::S_WATER;
	toughness = 0;

	transparent = true;

	currentChunk = WorldManager::instance->GetChunk(position.x, position.z); // cache this
}

std::vector<glm::vec3> Water::GetFreeSpaces(glm::vec3 _pos)
{
	std::vector<glm::vec3> freeSpaces = {};

	// Get our current chunk

	if (!DoesBlockExist(_pos - glm::vec3(0, 1, 0)) && !source)
		return {};

	if (!DoesBlockExist(_pos + glm::vec3(0, 0, 1)))
		freeSpaces.push_back(_pos + glm::vec3(0, 0, 1));

	if (!DoesBlockExist(_pos + glm::vec3(0, 0, -1)))
		freeSpaces.push_back(_pos + glm::vec3(0, 0, -1));

	if (!DoesBlockExist(_pos + glm::vec3(1, 0, 0)))
		freeSpaces.push_back(_pos + glm::vec3(1, 0, 0));

	if (!DoesBlockExist(_pos + glm::vec3(-1, 0, 0)))
		freeSpaces.push_back(_pos + glm::vec3(-1, 0, 0));

	return freeSpaces;
}

bool Water::DoesBlockExist(glm::vec3 _pos)
{
	if (currentChunk == nullptr)
		return false;

	Chunk* c = WorldManager::instance->GetChunk(_pos.x, _pos.z);

	int type = c->GetBlock(_pos.x, _pos.y, _pos.z);
	return type > 0 && type != WATER;
}

void Water::PlaceWater(glm::vec3 _pos, int _strength)
{
	if (_strength <= 0)
		return;

	Chunk* c = WorldManager::instance->GetChunk(_pos.x, _pos.z);

	Water* b = (Water*)c->CreateBlock(_pos.x, _pos.y, _pos.z, WATER);

	b->position -= c->position; // this gets added in CreateBlock

	b->source = false;
	b->strength = _strength;

	c->PlaceBlock(_pos.x, _pos.y, _pos.z, b);

	changedBlocks = true;
}

void Water::Update(int tick) // water functionality
{
	if (strength == 0 || tick % 80 != 0 || currentChunk == nullptr)
		return;

	std::vector<glm::vec3> freeSpaces = GetFreeSpaces(position);

	if (freeSpaces.size() == 0)
	{
		// go down
		if (currentChunk->DoesBlockExist(position.x, position.y - 1, position.z))
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