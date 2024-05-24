#ifndef _GAMEPLAY_H
#define _GAMEPLAY_H

#include <Objects/Scene.h>
#include <Objects/2DSprite.h>
#include <Objects/3DSprite.h>
#include <BS_thread_pool.hpp>
#include "../Objects/Base/Hud.h"
#include "../WorldManager.h"
#include "../DroppedItemManager.h"
#include "../MobManager.h"
#include <OpenGL/Model.h>

class Gameplay : public Scene
{
public:
	bool recordBlockData = false;
	std::map<std::string, int> blockData;
	glm::vec3 firstBlock = glm::vec3(0, 0, 0);
	bool showHud = true;
	DroppedItemManager* dim;
	MobManager* mm;
	WorldManager* wm;
	Hud* hud;
	Player* player;
	Camera2D* c2d = nullptr;
	Camera2D* credits = nullptr;

	Sprite3D* celestialSun;
	Sprite3D* celestialMoon;
	Sprite3D* celestialStars;

	float lastUpdate = 0.0f;
	float lastSecond = 0.0f;
	std::vector<float> tickTimes;
	float tps = 20.0f;
	int ticks = 0;
	int chunksLoaded = 0;
	int chunksRendered = 0;
	int regionsLoaded = 0;
	int lastTickSecond = 0;

	BS::thread_pool loadPool;

	std::deque<Chunk*> allChunks;

	Gameplay(WorldManager* _wm);

	void Create() override;
	void Draw() override;

	void QueueLoad(Chunk* c);
	void QueueLoadBlocks(Chunk* c);
	void QueueShadow(Chunk* c);

	void UpdateChunks();

	void UnloadChunk(Chunk* c);

	void KeyPress(int key) override;
	void MouseClick(int button, glm::vec2 mPos) override;
	void MouseRelease(int button, glm::vec2 mPos) override;
	void OnScroll(double x, double y) override;

	void FocusChange(bool focus) override;

	void Destroy() override;

	void Resize(float _w, float _h) override;
};

#endif