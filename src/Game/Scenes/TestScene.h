#ifndef _TESTSCENE_H
#define _TESTSCENE_H

#pragma once

#include <Game.h>
#include "../WorldManager.h"
#include <Objects/2DCamera.h>

class TestScene : public Scene
{
	WorldManager* wm;
	void Create() override;
	void Draw() override;
	void Destroy() override;
};

#endif