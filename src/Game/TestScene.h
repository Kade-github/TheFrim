#ifndef _TESTSCENE_H
#define _TESTSCENE_H

#pragma once

#include "../Engine/Objects/Scene.h"

class TestScene : public Scene
{
	void MouseMove(float x, float y) override;

	void Draw() override;
};

#endif