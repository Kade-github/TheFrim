#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include <Objects/2DSprite.h>
#include "../Objects/Base/Player.h"
#include "../WorldManager.h"

class Gameplay : public Scene
{
public:
	WorldManager* wm;
	Player* player;
	Camera2D* c2d;

	std::thread _generateThread;

	Gameplay(WorldManager* _wm);

	void Create() override;
	void Draw() override;

	void UpdateChunks();

	void KeyPress(int key) override;
	void MouseClick(int button, glm::vec2 mPos) override;

	void Destroy() override;
};

#endif