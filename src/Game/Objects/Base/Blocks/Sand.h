#ifndef _SANDBLOCK_H
#define _SANDBLOCK_H

#include "../Block.h"

class Sand : public Block
{
public:
	Sand(glm::vec3 _position) : Block(_position, BlockType::SAND) {
		position = _position;

		soundType = SoundType::S_SAND;
		toughness = 1.4;
	}

	BlockFace CreateFrontFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("sand");

		std::vector<GameObject::VVertex> frontVertices = CreateQuad(position, glm::vec3(1, 1, 0), 0, side);

		return BlockFace(frontVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBackFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("sand");

		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		std::vector<GameObject::VVertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(backVertices, indices);
	}

	BlockFace CreateLeftFace() override
	{
		glm::vec4 side = t->spriteSheet.GetUVFlip("sand");

		std::vector<GameObject::VVertex> leftVertices = CreateQuad(position + glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), 0, side);

		return BlockFace(leftVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateRightFace() override
	{
		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		glm::vec4 side = t->spriteSheet.GetUVFlip("sand");

		std::vector<GameObject::VVertex> rightVertices = CreateQuad(position, glm::vec3(0, 1, 1), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(rightVertices, indices);
	}

	BlockFace CreateTopFace() override
	{
		glm::vec4 dirt = t->spriteSheet.GetUVFlip("sand");

		std::vector<GameObject::VVertex> topVertices = CreateQuad(position + glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1, dirt);

		return BlockFace(topVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBottomFace() override
	{
		glm::vec4 dirt = t->spriteSheet.GetUVFlip("sand");

		std::vector<GameObject::VVertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, dirt);

		return BlockFace(bottomVertices, { 0, 1, 3, 1, 2, 3 });
	}
};

#endif