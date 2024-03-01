#ifndef _BLOCK_H
#define _BLOCK_H

#include <glm/glm.hpp>
#include <vector>

struct VVertex {
public:
	glm::vec3 position;
	glm::vec2 uv;

	VVertex(glm::vec3 _position, glm::vec2 _uv)
	{
		position = _position;
		uv = _uv;
	}
};

class BlockFace {
public:
	glm::vec3 sum;
	std::vector<VVertex> vertices;
	std::vector<unsigned int> indices;

	BlockFace()
	{
		vertices = {};
		indices = {};
		sum = glm::vec3(0, 0, 0);
	}

	BlockFace(std::vector<VVertex> _vertices, std::vector<unsigned int> _indices)
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
public:
	glm::vec3 position;
	BlockType type;


	bool changed = false;

	float blockWidth = 128;
	float blockHeight = 128;

	float textureWidth = 128;
	float textureHeight = 128;

	Block(glm::vec3 _position, BlockType _type);

	std::vector<VVertex> CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv);

	virtual BlockFace CreateFrontFace();
	virtual BlockFace CreateBackFace();
	virtual BlockFace CreateLeftFace();
	virtual BlockFace CreateRightFace();
	virtual BlockFace CreateTopFace();
	virtual BlockFace CreateBottomFace();

	glm::vec4 GetUV(int x, int y);
	glm::vec4 GetUVVerticallyFlipped(int x, int y);

};

#endif