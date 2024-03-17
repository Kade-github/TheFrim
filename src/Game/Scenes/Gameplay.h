#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include <Objects/2DSprite.h>
#include <BS_thread_pool.hpp>
#include "../Objects/Base/Player.h"
#include "../WorldManager.h"

class Gameplay : public Scene
{
public:
	WorldManager* wm;
	Player* player;
	Camera2D* c2d;

	Sprite2D* crosshair;

	BS::thread_pool loadPool;

	Gameplay(WorldManager* _wm);

	void Create() override;
	void Draw() override;

	void QueueLoad(Chunk* c);
	void QueueLoadBlocks(Chunk* c);
	void QueueShadow(Chunk* c);

	void UpdateChunks();

	void KeyPress(int key) override;
	void MouseClick(int button, glm::vec2 mPos) override;

	void Destroy() override;
};

#endif