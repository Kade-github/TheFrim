#ifndef _WOODBLOCK_H
#define _WOODBLOCK_H

#include "../Block.h"

class Wood : public Block
{
public:
	Wood(glm::vec3 _position) : Block(_position, BlockType::WOOD) {
		position = _position;

		soundType = SoundType::S_WOOD;
		toughness = 0.3f;
	}

	BlockFace CreateFrontFace() override
	{
		glm::vec4 side = GetUVVerticallyFlipped(BUV_WOOD);

		std::vector<GameObject::VVertex> frontVertices = CreateQuad(position, glm::vec3(1, 1, 0), 0, side);

		return BlockFace(frontVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBackFace() override
	{
		glm::vec4 side = GetUVVerticallyFlipped(BUV_WOOD);

		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		std::vector<GameObject::VVertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(backVertices, indices);
	}

	BlockFace CreateLeftFace() override
	{
		glm::vec4 side = GetUVVerticallyFlipped(BUV_WOOD);

		std::vector<GameObject::VVertex> leftVertices = CreateQuad(position + glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), 0, side);

		return BlockFace(leftVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateRightFace() override
	{
		std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

		glm::vec4 side = GetUVVerticallyFlipped(BUV_WOOD);

		std::vector<GameObject::VVertex> rightVertices = CreateQuad(position, glm::vec3(0, 1, 1), 0, side);

		std::swap(indices[0], indices[1]);
		std::swap(indices[3], indices[4]);

		return BlockFace(rightVertices, indices);
	}

	BlockFace CreateTopFace() override
	{
		glm::vec4 dirt = GetUVVerticallyFlipped(BUV_WOOD);

		std::vector<GameObject::VVertex> topVertices = CreateQuad(position + glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1, dirt);

		return BlockFace(topVertices, { 0, 1, 3, 1, 2, 3 });
	}

	BlockFace CreateBottomFace() override
	{
		glm::vec4 dirt = GetUVVerticallyFlipped(BUV_WOOD);

		std::vector<GameObject::VVertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, dirt);

		return BlockFace(bottomVertices, { 0, 1, 3, 1, 2, 3 });
	}
};

#endif