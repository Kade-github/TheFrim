#ifndef _WATERBLOCK_H
#define _WATERBLOCK_H

#include "../Chunk.h"

class Water : public Block
{
public:
	Chunk* currentChunk;

	int strength = 8;
	bool source = false;

	Water(glm::vec3 _position, int strength, bool isSource);

	BlockFace CreateFrontFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("water");

		std::vector<GameObject::VVertex> frontVertices = CreateQuad(position, glm::vec3(1, 1, 0), 0, side);

		return BlockFace(frontVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBackFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("water");

		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		std::vector<GameObject::VVertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(backVertices, indices);
	}

	BlockFace CreateLeftFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("water");

		std::vector<GameObject::VVertex> leftVertices = CreateQuad(position + glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), 0, side);

		return BlockFace(leftVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateRightFace() override
	{
		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		glm::vec4 side = t->spriteSheet.GetUVFlip("water");;

		std::vector<GameObject::VVertex> rightVertices = CreateQuad(position, glm::vec3(0, 1, 1), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(rightVertices, indices);
	}

	BlockFace CreateTopFace() override
	{
		glm::vec4 dirt = t->spriteSheet.GetUVFlip("water");

		std::vector<GameObject::VVertex> topVertices = CreateQuad(position + glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1, dirt);

		return BlockFace(topVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBottomFace() override
	{
		glm::vec4 dirt = t->spriteSheet.GetUVFlip("water");

		std::vector<GameObject::VVertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, dirt);

		return BlockFace(bottomVertices, { 0, 1, 3, 1, 2, 3 });
	}

	std::vector<glm::vec3> GetFreeSpaces(glm::vec3 _pos);

	bool DoesBlockExist(glm::vec3 _pos);

	void PlaceWater(glm::vec3 _pos, int _strength);

	bool Update(int tick) override;
};

#endif