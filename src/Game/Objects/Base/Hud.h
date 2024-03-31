#ifndef _HUD_H
#define _HUD_H

#include <Objects/GameObject.h>
#include <Objects/2DCamera.h>
#include <Objects/2DSprite.h>
#include "Player.h"

class Hud : public GameObject
{
private:
	Player* player;
	Camera2D* c2d;
	Texture* h;

	glm::vec4 GetHudSrc(float x, float y);

public:
	Sprite2D* crosshair;

	std::vector<Sprite2D*> hotbar;
	std::vector<Sprite2D*> hotbarItems;
	std::vector<Sprite2D*> hearts;
	std::vector<Sprite2D*> armor;

	void UpdateHotbar();
	void UpdateHearts();
	void UpdateArmor();

	Hud(glm::vec3 _pos, Player* _p, Camera2D* _c2d);

	~Hud();

	void Draw() override;
};

#endif