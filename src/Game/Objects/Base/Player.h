#ifndef _PLAYER_H
#define _PLAYER_H

#include "Entity.h"
#include "Chunk.h"

class Player : public Entity
{
	unsigned int VAO, VBO, EBO;

	std::vector<VVertex> vertices;
	std::vector<unsigned int> indices;

	float headBop = 0;

	Block* selectedBlock = nullptr;
	BlockFace selectedFace;

	void DrawBlockBreak(BlockFace& f);

	void SetBreakBuffer();

	void RenderBreak();

public:
	float jumpCooldown = 0;

	Player(glm::vec3 pos);
	~Player();
	void Draw() override;

	bool freeCam = false;

	void MouseClick(int button, glm::vec2 mPos) override;

	void KeyPress(int key) override;
};

#endif