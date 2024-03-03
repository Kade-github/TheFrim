#ifndef _DIRTBLOCK_H
#define _DIRTBLOCK_H

#include "../Block.h"

class Dirt : public Block
{
public:
	Dirt(glm::vec3 _position) : Block(_position, BlockType::DIRT) {
		position = _position;
	}

	BlockFace CreateFrontFace() override
	{
		glm::vec4 side = GetUV(0, 2);

		std::vector<VVertex> frontVertices = CreateQuad(position, glm::vec3(1.01f, 1.01f, 0), 0, side);

		return BlockFace(frontVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBackFace() override
	{
		glm::vec4 side = GetUV(0, 2);

		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		std::vector<VVertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1.01f, 1.01f, 0), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(backVertices, indices);
	}

	BlockFace CreateLeftFace() override
	{
		glm::vec4 side = GetUV(0, 2);

		std::vector<VVertex> leftVertices = CreateQuad(position + glm::vec3(1, 0, 0), glm::vec3(0, 1.01f, 1.01f), 0, side);

		return BlockFace(leftVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateRightFace() override
	{
		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		glm::vec4 side = GetUV(0, 2);

		std::vector<VVertex> rightVertices = CreateQuad(position, glm::vec3(0, 1.01f, 1.01f), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(rightVertices, indices);
	}

	BlockFace CreateTopFace() override
	{
		glm::vec4 dirt = GetUV(0, 2);

		std::vector<VVertex> topVertices = CreateQuad(position + glm::vec3(0, 1, 0), glm::vec3(1.01f, 0, 0), 1, dirt);

		return BlockFace(topVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBottomFace() override
	{
		glm::vec4 dirt = GetUV(0, 2);

		std::vector<VVertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1.01f, 0, 0), -1, dirt);

		return BlockFace(bottomVertices, { 0, 1, 3, 1, 2, 3 });
	}
};

#endif