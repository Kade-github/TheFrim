#ifndef _WORLDS_H
#define _WORLDS_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include "../Objects/Menu/Bar.h"
#include "../WorldManager.h"

class Worlds : public Scene
{
public:
	int page = 0;

	Camera2D* c2d;

	std::vector<Data::World> worlds;

	Bar* createWorld;
	Bar* goBack;

	void Create() override;

	void Resize(float _w, float _h) override;

	void Draw() override;
	void MouseClick(int button, glm::vec2 mPos) override;
};


#endif