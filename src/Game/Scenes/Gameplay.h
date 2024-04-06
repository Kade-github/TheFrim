#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include <Objects/Scene.h>
#include <Objects/2DSprite.h>
#include <BS_thread_pool.hpp>
#include "../Objects/Base/Hud.h"
#include "../WorldManager.h"

class Gameplay : public Scene
{
public:
	WorldManager* wm;
	Hud* hud;
	Player* player;
	Camera2D* c2d;

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
	void MouseRelease(int button, glm::vec2 mPos) override;
	void OnScroll(double x, double y) override;

	void Destroy() override;
};

#endif