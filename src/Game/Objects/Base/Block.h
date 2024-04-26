#ifndef _BLOCK_H
#define _BLOCK_H

#include <Objects/GameObject.h>
#include <glm/glm.hpp>
#include <vector>
#include "../../Data/World.h"
#include <OpenGL/Texture.h>

struct BlockFace {
	glm::vec3 position;
	glm::vec3 sum;
	std::vector<GameObject::VVertex> vertices = {};
	std::vector<unsigned int> indices = {};
	int type = 0;
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

	~BlockFace()
	{
		vertices = {};
		indices = {};
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
	GLASS = 12,
	WATER = 13,
	BEDROCK = 14,
	COAL_ORE = 15,
	IRON_ORE = 16,
	GOLD_ORE = 17,
	DIAMOND_ORE = 18,
};

enum SoundType {
	S_GRASS = 0,
	S_STONE = 1,
	S_WOOD = 2,
	S_WATER = 3,
	S_SAND = 4,
	S_GLASS = 5
};

class Block
{
	int GetBreakTexture();
	glm::vec4 GetBreakUV();

public:
	Data::BlockData data;
	glm::vec3 position;
	BlockType type;

	bool changedBlocks = false;

	bool transparent = false;
	bool isInteractable = false;

	float breakProgress = 0;
	float toughness = 1;

	std::vector<BlockFace> faces = {};

	SoundType soundType = SoundType::S_GRASS;

	bool changed = false;

	int blockWidth = 128;
	int blockHeight = 128;

	Texture* t;

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

	~Block();

	virtual void OnInteract() {};

	virtual bool Update(int tick) { return true; };

	void Draw(std::vector<GameObject::VVertex>& verts, std::vector<unsigned int>& inds);

};

#endif