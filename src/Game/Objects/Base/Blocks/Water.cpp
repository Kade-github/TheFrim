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

	data.tags.clear();

	data.AddTag("strength", std::to_string(strength));
	data.AddTag("source", isSource ? "true" : "false");

	currentChunk = WorldManager::instance->GetChunk(position.x, position.z); // cache this
}

std::vector<glm::vec3> Water::GetFreeSpaces(glm::vec3 _pos)
{
	std::vector<glm::vec3> freeSpaces = {};

	if (source)
	{
		if (!DoesBlockExist(_pos + glm::vec3(0, 0, 1)))
			freeSpaces.push_back(_pos + glm::vec3(0, 0, 1));

		if (!DoesBlockExist(_pos + glm::vec3(0, 0, -1)))
			freeSpaces.push_back(_pos + glm::vec3(0, 0, -1));

		if (!DoesBlockExist(_pos + glm::vec3(1, 0, 0)))
			freeSpaces.push_back(_pos + glm::vec3(1, 0, 0));

		if (!DoesBlockExist(_pos + glm::vec3(-1, 0, 0)))
			freeSpaces.push_back(_pos + glm::vec3(-1, 0, 0));

		if (!DoesBlockExist(_pos + glm::vec3(0, -1, 0)))
			freeSpaces.push_back(_pos + glm::vec3(0, -1, 0));
	}
	else
	{
		if (!DoesBlockExist(_pos + glm::vec3(0, -1, 0)))
		{
			freeSpaces.push_back(_pos + glm::vec3(0, -1, 0));
			return freeSpaces;
		}

		if (!DoesBlockExist(_pos + glm::vec3(0, 0, 1)))
			freeSpaces.push_back(_pos + glm::vec3(0, 0, 1));

		if (!DoesBlockExist(_pos + glm::vec3(0, 0, -1)))
			freeSpaces.push_back(_pos + glm::vec3(0, 0, -1));

		if (!DoesBlockExist(_pos + glm::vec3(1, 0, 0)))
			freeSpaces.push_back(_pos + glm::vec3(1, 0, 0));

		if (!DoesBlockExist(_pos + glm::vec3(-1, 0, 0)))
			freeSpaces.push_back(_pos + glm::vec3(-1, 0, 0));
	}

	return freeSpaces;
}

bool Water::DoesBlockExist(glm::vec3 _pos)
{
	Chunk* c = WorldManager::instance->GetChunk(_pos.x, _pos.z);

	if (c == nullptr)
		return false;

	int type = c->GetBlock(_pos.x, _pos.y, _pos.z);
	return type > 0;
}

void Water::PlaceWater(glm::vec3 _pos, int _strength)
{
	if (_strength <= 0)
		return;

	Chunk* c = WorldManager::instance->GetChunk(_pos.x, _pos.z);

	Data::BlockData d = data;
	d.tags.clear();

	d.AddTag("strength", std::to_string(_strength));
	d.AddTag("source", "false");

	Water* b = (Water*)c->CreateBlock(_pos.x, _pos.y, _pos.z, WATER, d);

	b->position -= c->position; // this gets added in CreateBlock

	b->source = false;
	b->strength = _strength;

	b->data = d;

	Game::instance->log->Write((source ? "[Source Block] [" : " [") + std::to_string(strength) + "] Placed water at " + std::to_string(_pos.x) + ", " + std::to_string(_pos.y) + ", " + std::to_string(_pos.z));

	c->PlaceBlock(_pos.x, _pos.y, _pos.z, b);

	changedBlocks = true;
}

void Water::Update(int tick) // water functionality
{
	if (strength == 0 || tick % 10 != 0 || currentChunk == nullptr)
		return;

	std::vector<glm::vec3> freeSpaces = GetFreeSpaces(position);

	for (glm::vec3 freeSpace : freeSpaces)
	{
		Chunk* c = WorldManager::instance->GetChunk(freeSpace.x, freeSpace.z);

		PlaceWater(freeSpace, strength - 1);
	}

}