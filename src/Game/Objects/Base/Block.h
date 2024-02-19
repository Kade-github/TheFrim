#ifndef _BLOCK_H
#define _BLOCK_H

#include "../../Include/glm/glm.hpp"
#include <vector>

class Vertex {
public:
	glm::vec3 position;
	glm::vec2 uv;

	Vertex(glm::vec3 _position, glm::vec2 _uv)
	{
		position = _position;
		uv = _uv;
	}
};

class BlockFace {
public:
	glm::vec3 sum;
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	BlockFace()
	{
		vertices = {};
		indices = {};
		sum = glm::vec3(0, 0, 0);
	}

	BlockFace(std::vector<Vertex> _vertices, std::vector<unsigned int> _indices)
	{
		vertices = _vertices;
		indices = _indices;
		sum = glm::vec3(0, 0, 0);
	}

	bool operator==(const BlockFace& other) const
	{
		return sum.x == other.sum.x && sum.y == other.sum.y && sum.z == other.sum.z;
	}
};

enum BlockType
{
	DIRT,
	GRASS
};

class Block
{
	std::vector<Vertex> CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv);
public:
	glm::vec3 position;
	BlockType type;

	std::vector<BlockFace> faces;

	Block(glm::vec3 _position, BlockType _type);

	void CreateFaces();

	BlockFace GetFrontFace();
	BlockFace GetBackFace();
	BlockFace GetLeftFace();
	BlockFace GetRightFace();
	BlockFace GetTopFace();
	BlockFace GetBottomFace();
};

#endif