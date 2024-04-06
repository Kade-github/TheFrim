#ifndef _HUD_H
#define _HUD_H

#include <Objects/GameObject.h>
#include <Objects/2DCamera.h>
#include "UI/ItemUI.h"
#include "UI/Inventory.h"
#include "Player.h"

class Hud : public GameObject
{
private:
	Player* player;
	Camera2D* c2d;

	Sprite2D* hand;

	Texture* h;
	Texture* i;

	float _heartUpdate = 0;

public:
	Inventory* inv;

	int selected = 0;

	Sprite2D* crosshair;

	std::vector<Sprite2D*> hotbar;
	std::vector<ItemUI*> hotbarItems;
	std::vector<Sprite2D*> hearts;
	std::vector<Sprite2D*> armor;

	void SetSelected(int s);

	void UpdateHotbar();
	void UpdateHearts();
	void UpdateArmor();

	Hud(glm::vec3 _pos, Player* _p, Camera2D* _c2d);

	~Hud();

	void Draw() override;
};

#endif