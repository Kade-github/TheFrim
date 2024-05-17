#ifndef _SETTINGS_MENU_H
#define _SETTINGS_MENU_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include "../Objects/Menu/Bar.h"
#include "../Objects/Menu/DragBar.h"

class SettingsMenu : public Scene
{
public:
	Camera2D* c2d;

	Sprite2D* background;

	Bar* vsync;
	Bar* fullscreen;
	Bar* ambientDiffuse;

	DragBar* fov;
	DragBar* renderDistance;
	DragBar* fogDistance;
	DragBar* masterVolume;

	Bar* back;

	void Create() override;

	void Draw() override;
	void MouseClick(int button, glm::vec2 mPos) override;
	void MouseRelease(int button, glm::vec2 mPos) override;
	void Resize(float _w, float _h) override;

	void Destroy() override;
};

#endif