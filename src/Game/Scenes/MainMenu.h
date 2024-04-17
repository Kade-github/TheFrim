#ifndef _MAINMENU_H
#define _MAINMENU_H

#pragma once

#include <Game.h>
#include <Objects/2DCamera.h>
#include "../Objects/Menu/Bar.h"

class MainMenu : public Scene
{
public:
	Camera2D* c2d;

	Sprite2D* background;
	Sprite2D* logo;

	bool mouseSelected = false;
	int selectedIndex = 0;
	float fakeIndex = 0;

	Bar* selectWorld;
	Bar* settings;
    Bar* blockTest;
	Bar* exit;

	void Create() override;

	void Draw() override;
	void MouseClick(int button, glm::vec2 mPos) override;
	void Resize(float _w, float _h) override;
};

#endif