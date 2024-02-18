#ifndef _TESTSCENE_H
#define _TESTSCENE_H

#pragma once

#include "../Engine/Objects/Scene.h"

class TestScene : public Scene
{
	void Create() override;
	void Draw() override;
};

#endif