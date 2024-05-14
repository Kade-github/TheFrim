#include "Water.h"
#include <Game.h>
#include "../Chunk.h"
#include "../../../WorldManager.h"

Water::Water(glm::vec3 _position, int strength, bool isSource) : Block(_position, BlockType::WATER) {
	position = _position;

	soundType = SoundType::S_WATER;
	toughness = 0;

	transparent = true;

	this->strength = strength;
	source = isSource;
	updateable = true;

	currentChunk = WorldManager::instance->GetChunk(position.x, position.z); // cache this
}

std::vector<glm::vec3> Water::GetFreeSpaces(glm::vec3 _pos)
{
	std::vector<glm::vec3> freeSpaces = {};

	if (!DoesBlockExist(_pos + glm::vec3(0, -1, 0)))
	{
		if (!lastWaterBlocked)
			freeSpaces.push_back(_pos + glm::vec3(0, -1, 0));
	}
	else if (!lastWaterBlocked)
	{
		if (!DoesBlockExist(_pos + glm::vec3(0, 0, 1)))
		{
			if (!lastWaterBlocked)
				freeSpaces.push_back(_pos + glm::vec3(0, 0, 1));
		}

		if (!DoesBlockExist(_pos + glm::vec3(1, 0, 0)))
		{
			if (!lastWaterBlocked)
				freeSpaces.push_back(_pos + glm::vec3(1, 0, 0));
		}

		if (!DoesBlockExist(_pos + glm::vec3(-1, 0, 0)))
		{
			if (!lastWaterBlocked)
				freeSpaces.push_back(_pos + glm::vec3(-1, 0, 0));
		}

		if (!DoesBlockExist(_pos + glm::vec3(0, 0, -1)))
		{
			if (!lastWaterBlocked)
				freeSpaces.push_back(_pos + glm::vec3(0, 0, -1));
		}
	}

	return freeSpaces;
}

bool Water::DoesBlockExist(glm::vec3 _pos)
{
	lastWaterBlocked = false;

	Chunk* c = WorldManager::instance->GetChunk(_pos.x, _pos.z);

	if (c == nullptr)
		return true;

	glm::vec3 rPos = _pos - c->position;

	int b = c->GetBlock(rPos.x, rPos.y, rPos.z);

	if (b <= 0)
		return false;

	if (b == WATER)
		lastWaterBlocked = true;

	return true;
}

void Water::PlaceWater(Chunk* c, glm::vec3 _pos, int _strength)
{
	if (_pos.y < position.y)
		_strength += 2;

	if (_strength <= 0)
		return;

	if (c == nullptr)
		return;

	Data::BlockData d = data;
	d.tags.clear();

	d.AddTag("strength", std::to_string(_strength));
	d.AddTag("source", "false");

	Water* b = (Water*)c->CreateBlock(_pos.x, _pos.y, _pos.z, WATER, d);

	b->position -= c->position; // this gets added in CreateBlock

	b->source = false;
	b->strength = _strength;

	b->data = d;

	c->PlaceBlock(_pos.x, _pos.y, _pos.z, b);

	changedBlocks = true;
}

bool Water::Update(int tick) // water functionality
{
	if (strength == 0 || currentChunk == nullptr)
		return true;

	if (tick % 8 != 0)
		return false;

	std::vector<glm::vec3> freeSpaces = GetFreeSpaces(position);

	for (glm::vec3 freeSpace : freeSpaces)
	{
		Chunk* c = WorldManager::instance->GetChunk(freeSpace.x, freeSpace.z);

		PlaceWater(c, freeSpace, strength - 1);
	}

	return true;
}