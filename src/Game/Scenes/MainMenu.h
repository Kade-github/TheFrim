#ifndef _MAINMENU_H
#define _MAINMENU_H

#pragma once

#include <Game.h>
#include "../WorldManager.h"
#include <Objects/2DCamera.h>
#include "../Objects/Menu/Bar.h"

class MainMenu : public Scene
{
public:
	Camera2D* c2d;

	Sprite2D* background;
	Sprite2D* logo;

	int selectedIndex = 0;
	float fakeIndex = 0;

	Bar* selectWorld;
	Bar* settings;
	Bar* exit;

	void Create() override;

	void Draw() override;
	void KeyPress(int key) override;

	void Resize(float _w, float _h) override;
};

#endif