#ifndef _LOADINGWORLD_H
#define _LOADINGWORLD_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include "../WorldManager.h"


class LoadingWorld : public Scene
{
	std::string _name;
public:
	WorldManager* wm;

	Text2D* label;

	LoadingWorld(std::string name);

	Camera2D* c2d;

	void Create() override;

	void Resize(float _w, float _h) override;

	void Draw() override;
};

#endif