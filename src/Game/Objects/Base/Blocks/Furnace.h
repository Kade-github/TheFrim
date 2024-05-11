#ifndef _FURNACEBLOCK_H
#define _FURNACEBLOCK_H

#include "../Block.h"
#include "../../../WorldManager.h"
class Furnace : public Block
{
	bool check = true;
	Chunk* currentChunk = nullptr;
public:
	float tickPerc = 0.0f;

	int ticksNeeded = -1;
	int ticks = -1;
	int cookingType = -1;

	bool light = false;

	Furnace(glm::vec3 _position) : Block(_position, BlockType::FURNACE) {
		position = _position;

		soundType = SoundType::S_STONE;
		toughness = 0.3f;
		isInteractable = true;

		currentChunk = WorldManager::instance->GetChunk(position.x, position.z);
	}

	BlockFace CreateFrontFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("furnace");

		std::vector<GameObject::VVertex> frontVertices = CreateQuad(position, glm::vec3(1, 1, 0), 0, side);

		return BlockFace(frontVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBackFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("cobblestone");

		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		std::vector<GameObject::VVertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(backVertices, indices);
	}

	BlockFace CreateLeftFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("cobblestone");

		std::vector<GameObject::VVertex> leftVertices = CreateQuad(position + glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), 0, side);

		return BlockFace(leftVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateRightFace() override
	{
		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		glm::vec4 side = t->spriteSheet.GetUVFlip("cobblestone");

		std::vector<GameObject::VVertex> rightVertices = CreateQuad(position, glm::vec3(0, 1, 1), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(rightVertices, indices);
	}

	BlockFace CreateTopFace() override
	{
		glm::vec4 dirt = t->spriteSheet.GetUVFlip("cobblestone");

		std::vector<GameObject::VVertex> topVertices = CreateQuad(position + glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1, dirt);

		return BlockFace(topVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBottomFace() override
	{
		glm::vec4 dirt = t->spriteSheet.GetUVFlip("cobblestone");

		std::vector<GameObject::VVertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, dirt);

		return BlockFace(bottomVertices, { 0, 1, 3, 1, 2, 3 });
	}

	void OnInteract() override;

	int GetTicksForItem(int item);
	int GetOutputForItem(int item);

	bool Update(int tick) override;

	void Destroy() override;
};

#endif