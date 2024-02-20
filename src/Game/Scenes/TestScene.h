#ifndef _TESTSCENE_H
#define _TESTSCENE_H

#pragma once

#include <Game.h>
#include "../WorldManager.h"

class TestScene : public Scene
{
	WorldManager* wm;

	void Create() override;
	void Draw() override;
};

#endif