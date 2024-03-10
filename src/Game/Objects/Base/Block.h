#ifndef _BLOCK_H
#define _BLOCK_H

#include <Objects/GameObject.h>
#include <glm/glm.hpp>
#include <vector>

class BlockFace {
public:
	glm::vec3 sum;
	std::vector<GameObject::VVertex> vertices;
	std::vector<unsigned int> indices;

	BlockFace()
	{
		vertices = {};
		indices = {};
		sum = glm::vec3(0, 0, 0);
	}

	BlockFace(std::vector<GameObject::VVertex> _vertices, std::vector<unsigned int> _indices)
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
	GRASS,
	STONE,
	COBBLESTONE,
	SAND,
	WOOD,
	WOODENPLANKS,
	LEAVES,
	CRAFTINGTABLE,
	FURNACE,
	CLAY,
	GLASS
};

class Block
{
public:
	glm::vec3 position;
	BlockType type;

	std::vector<BlockFace> faces = {};

	bool changed = false;

	float blockWidth = 128;
	float blockHeight = 128;

	float textureWidth = 128;
	float textureHeight = 128;

	Block(glm::vec3 _position, BlockType _type);

	std::vector<GameObject::VVertex> CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv);

	virtual BlockFace CreateFrontFace();
	virtual BlockFace CreateBackFace();
	virtual BlockFace CreateLeftFace();
	virtual BlockFace CreateRightFace();
	virtual BlockFace CreateTopFace();
	virtual BlockFace CreateBottomFace();

	glm::vec4 GetUV(int x, int y);
	glm::vec4 GetUVVerticallyFlipped(int x, int y);

	void Draw(std::vector<GameObject::VVertex>& verts, std::vector<unsigned int>& inds);

};

#endif