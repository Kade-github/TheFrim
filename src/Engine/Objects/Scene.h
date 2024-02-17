#ifndef _SCENE_H
#define _SCENE_H

#pragma once

#include "GameObject.h"

#include <vector>

class Scene
{
public:
	std::vector<GameObject*> objects;
};

#endif