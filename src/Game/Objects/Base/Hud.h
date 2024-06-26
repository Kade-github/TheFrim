#ifndef _HUD_H
#define _HUD_H

#include <Objects/GameObject.h>
#include <Objects/2DCamera.h>
#include "UI/ItemUI.h"
#include "UI/Inventory.h"
#include "Player.h"

#include "../Menu/Bar.h"

class Hud : public GameObject
{
private:
	Player* player;
	Camera2D* c2d;

	Texture* h;
	Texture* i;


	float _heartUpdate = 0;

	bool _exiting = false;
	bool _askedForScreenshot = false;

public:
	Text2D* pauseHeader;
	Text2D* hintText;

	Sprite2D* pauseBackground;
	Sprite2D* waterOverlay;
	Sprite2D* deathOverlay;

	Bar* resume;
	Bar* title;

	Sprite3D* hand;

	static bool GamePaused;
	static bool endSequence;

	bool rocketScreen = false;

	Inventory* inv;

	int selected = 0;

	bool playEnd = false;

	Sprite2D* crosshair;
	bool hintShown = false;
	Sprite2D* hint;

	std::vector<Sprite2D*> hotbar;
	std::vector<ItemUI*> hotbarItems;
	std::vector<Sprite2D*> hearts;
	std::vector<Sprite2D*> air;
	std::vector<Sprite2D*> armor;

	void InventoryShown(bool s);
	void ShowCraftingTable(bool s);
	void ShowFurnace(bool s);
	void ShowPauseMenu(bool s);
	void ShowDeathScreen();

	void ShowRocketScreen();

	void ShowKnowledgementTablet(Data::InventoryItem& item);

	void SetSelected(int s);

	void UpdateHotbar();
	void UpdateHearts();
	void ClearAir();
	void UpdateAir();
	void UpdateArmor();

	float hintTime = 0;

	void ShowHint(std::string text);

	Hud(glm::vec3 _pos, Player* _p, Camera2D* _c2d);

	~Hud();

	void Draw() override;

	void MouseClick(int button, glm::vec2 pos) override;
};

#endif