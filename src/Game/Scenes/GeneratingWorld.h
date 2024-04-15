#ifndef _GENERATINGWORLD_H
#define _GENERATINGWORLD_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include "../WorldManager.h"


class GeneratingWorld : public Scene
{
	std::string _name;
	std::string _seed;
public:
	bool switched = false;
	WorldManager* wm;

	Text2D* label;

	GeneratingWorld(std::string name, std::string seed);

	Camera2D* c2d;

	void Create() override;

	void Resize(float _w, float _h) override;

	void Draw() override;
};

#endif