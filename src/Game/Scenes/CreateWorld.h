#ifndef _CREATEWORLD_H
#define _CREATEWORLD_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include "../Objects/Menu/InputBar.h"
#include "../Objects/Menu/Bar.h"

class CreateWorld : public Scene
{
public:
	Camera2D* c2d;

	Bar* createWorld;
	Bar* goBack;

	InputBar* name;
	InputBar* seed;

	void Create() override;

	void Resize(float _w, float _h) override;

	void Draw() override;
	void MouseClick(int button, glm::vec2 mPos) override;

	void OnChar(unsigned int c) override;
	void KeyPress(int key) override;
};

#endif