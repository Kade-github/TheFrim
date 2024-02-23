#ifndef _MAINMENU_H
#define _MAINMENU_H

#pragma once

#include <Game.h>
#include "../WorldManager.h"
#include <Objects/2DCamera.h>
#include <Objects/2DSprite.h>

class MainMenu : public Scene
{
public:
	Camera2D* c2d;

	Sprite2D* background;
	Sprite2D* logo;

	void Create() override;
	void Resize(float _w, float _h) override;
};

#endif