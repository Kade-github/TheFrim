#ifndef _WORLDS_H
#define _WORLDS_H

#include <Objects/Scene.h>
#include <Objects/2DCamera.h>
#include <Objects/2DRectangle.h>
#include "../Objects/Menu/Bar.h"
#include "../WorldManager.h"
#include "../Objects/Menu/World.h"

class Worlds : public Scene
{
	float _lastScroll = 0.0f;
public:

	float scrollModifier = 0;
	bool canScroll = false;
	Sprite2D* deepBackground;
	Camera2D* c2d;

	Rectangle2D* scrollBar;

	std::vector<Data::World> worlds;
	std::vector<World*> worldObjects;
	bool justDeleted = false;
	bool deleteWorld = false;

	Bar* createWorld;
	Bar* goBack;

	void Create() override;

	void Resize(float _w, float _h) override;

	void CreateWorldObjects();

	void LoadWorld(Data::World w);
	void DeleteWorld(Data::World w);

	void SetScroll();

	void Draw() override;
	void MouseClick(int button, glm::vec2 mPos) override;
	void OnScroll(double x, double y) override;

    void Destroy() override;
};


#endif