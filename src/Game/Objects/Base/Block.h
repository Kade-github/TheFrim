#ifndef _BLOCK_H
#define _BLOCK_H

#include <Objects/GameObject.h>
#include <glm/glm.hpp>
#include <vector>

class BlockFace {
public:
	glm::vec3 position;
	glm::vec3 sum;
	std::vector<GameObject::VVertex> vertices;
	std::vector<unsigned int> indices;

	int lightLevel = 0;

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

	void SetUV(glm::vec4 uv)
	{
		vertices[0].uv = uv;
		vertices[1].uv = glm::vec2(uv.x + uv.z, uv.y);
		vertices[2].uv = glm::vec2(uv.x, uv.y + uv.w);
		vertices[3].uv = glm::vec2(uv.x + uv.z, uv.y + uv.w);
	}

	bool operator==(const BlockFace& other) const
	{
		return sum.x == other.sum.x && sum.y == other.sum.y && sum.z == other.sum.z;
	}
};

enum BlockType
{
	DIRT = 1,
	GRASS = 2,
	STONE = 3,
	COBBLESTONE = 4,
	SAND = 5,
	WOOD = 6,
	WOODENPLANKS = 7,
	LEAVES = 8,
	CRAFTINGTABLE = 9,
	FURNACE = 10,
	CLAY = 11,
	GLASS = 12
};

enum SoundType {
	S_GRASS = 0,
	S_STONE = 1,
	S_WOOD = 2,
};

#define BUV_SHADOWFULL 0, 4
#define BUV_SHADOWSEVENTYFIVE 0, 1
#define BUV_SHADOWFIFTY 1, 0
#define BUV_SHADOWTWENTYFIVE 0, 0
#define BUV_BREAK0 1, 1
#define BUV_BREAK1 2, 0
#define BUV_BREAK2 2, 1
#define BUV_BREAK3 0, 2
#define BUV_DIRT 3, 0
#define BUV_GRASS 0, 3
#define BUV_GRASSSIDE 1, 3
#define BUV_STONE 4, 1
#define BUV_COBBLESTONE 1, 2
#define BUV_WOOD 4, 2
#define BUV_LEAVES 2, 3
#define BUV_WOODENPLANKS 4, 3

class Block
{
	int GetBreakTexture();
	glm::vec4 GetBreakUV();

public:
	glm::vec3 position;
	BlockType type;

	float breakProgress = 0;
	float toughness = 0;

	std::vector<BlockFace> faces = {};

	SoundType soundType = SoundType::S_GRASS;

	bool changed = false;

	int blockWidth = 128;
	int blockHeight = 128;

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

	BlockFace BreakFrontFace();
	BlockFace BreakBackFace();
	BlockFace BreakLeftFace();
	BlockFace BreakRightFace();
	BlockFace BreakTopFace();
	BlockFace BreakBottomFace();

	glm::vec4 GetUV(int x, int y);
	glm::vec4 GetUVVerticallyFlipped(int x, int y);

	void Draw(std::vector<GameObject::VVertex>& verts, std::vector<unsigned int>& inds);

};

#endif