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
	float _shake = 0;

	Block* selectedBlock = nullptr;
	BlockFace selectedFace;

	void DrawBlockBreak(BlockFace f);

	void SetBreakBuffer();

	void RenderBreak();

	void ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal);

public:
	Data::Player playerData;

	float jumpCooldown = 0;

	void CameraShake(float amount);

	void Hurt(float damage);
	void Heal(float amount);

	Player(glm::vec3 pos);
	~Player();
	void Draw() override;

	bool freeCam = false;

	void MouseClick(int button, glm::vec2 mPos) override;

	void KeyPress(int key) override;
};

#endif