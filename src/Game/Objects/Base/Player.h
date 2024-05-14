#ifndef _PLAYER_H
#define _PLAYER_H

#include "Entity.h"
#include "Chunk.h"

class Player : public Entity
{
	unsigned int VAO, VBO, EBO;

	std::vector<VVertex> vertices;
	std::vector<unsigned int> indices;

	bool _inInventory = false;

	float headBop = 0;
	float _shake = 0;

	float jumpedFrom = -1;
	int lastAirUpdate = 10;

	bool instantBreak = false;

	bool goToTop = false;

	void DrawBlockBreak(BlockFace f);

	void SetBreakBuffer();

	void RenderBreak();

	void ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal);

public:
	Data::Player playerData;
	Block* selectedBlock = nullptr;
	BlockFace selectedFace;

	bool noTarget = false;
	bool shift = false;
	glm::vec3 blockOnShift = glm::vec3(0, 0, 0);

	int lastBreakSfx = 0;
	float breakCooldown = 0;
	bool wasDead = false;
	bool dead = false;

	float jumpCooldown = 0;

	void CameraShake(float amount);

	void Hurt(float damage, glm::vec3 position = glm::vec3(0,0,0));
	void Heal(float amount);

	void ToggleInventory();
	void ToggleCraftingTable();
	void ToggleFurnace();

	void TogglePauseMenu();

	void SetItem(Data::InventoryItem item, int x, int y);

	Player(glm::vec3 pos);
	~Player();
	void Draw() override;

	bool freeCam = false;

	void MouseClick(int button, glm::vec2 mPos) override;

	void OnScroll(double x, double y) override;

	void KeyPress(int key) override;
};

#endif