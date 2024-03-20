#include "Worlds.h"
#include "MainMenu.h"
#include "CreateWorld.h"
#include "LoadingWorld.h"
#include <Helpers/Collision2D.h>

void Worlds::Create()
{
	worlds = WorldManager::GetWorlds();

	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	Sprite2D* background = new Sprite2D("Assets/Textures/MainMenu/background.png", glm::vec3(0, 0, 0));

	background->width = c2d->_w;
	background->height = c2d->_h;

	c2d->AddObject(background);

	deepBackground = new Sprite2D("Assets/Textures/WorldSelect/deep_background.png", glm::vec3(0, 0, 0));
	deepBackground->Resize(deepBackground->width * 1.5, deepBackground->height * 1.5);

	deepBackground->position.x = (c2d->_w / 2 ) - deepBackground->width / 2;
	deepBackground->position.y = ((c2d->_h / 2) + 50) - deepBackground->height / 2;

	c2d->AddObject(deepBackground);

	createWorld = new Bar(glm::vec3(0, 0, 0), "Create World");
	createWorld->Resize((createWorld->width * 0.8) - 100, createWorld->height * 0.8);

	createWorld->position.x = 250;
	createWorld->position.y = 50;

	c2d->AddObject(createWorld);

	goBack = new Bar(glm::vec3(0, 0, 0), "Go Back");
	goBack->Resize((goBack->width * 0.8) - 100, goBack->height * 0.8);

	goBack->position.x = c2d->_w - (250 + goBack->width);
	goBack->position.y = 50;

	c2d->AddObject(goBack);

	CreateWorldObjects();
}

void Worlds::Resize(float _w, float _h)
{
	c2d->Resize();
}

void Worlds::CreateWorldObjects()
{
	for (auto w : worldObjects)
	{
		c2d->RemoveObject(w);
		delete w;
	}

	for (int i = 0; i < worlds.size(); i++)
	{
		Data::World w = worlds[i];

		World* world = new World(glm::vec3(0, 0, 0));

		world->width *= 1.45;
		world->height *= 1.45;

		world->SetName(w.name);
		world->SetSeed(w.seed);

		world->position.x = deepBackground->position.x + (deepBackground->width / 2) - (world->width / 2);
		world->position.y = deepBackground->position.y + deepBackground->height - (world->height + 42) - (world->height * i);

		world->SetSelectCallback([&, i]() {
			LoadWorld(worlds[i]);
		});

		world->SetDeleteCallback([&, world, i]() {
			if (!deleteWorld)
			{
				deleteWorld = true;
				// give warning

				world->SetDeleteText("Really?");
			}
			else
			{
				DeleteWorld(worlds[i]);
				deleteWorld = false;
			}
		});

		c2d->AddObject(world);

		worldObjects.push_back(world);
	}
}

void Worlds::LoadWorld(Data::World w)
{
	Game::instance->SwitchScene(new LoadingWorld(w.name));
}

void Worlds::DeleteWorld(Data::World w)
{
	WorldManager::instance->DeleteWorld(w);
	worlds = WorldManager::GetWorlds();
	CreateWorldObjects();

	justDeleted = true;
}

void Worlds::Draw()
{


	Scene::Draw();
}

void Worlds::MouseClick(int button, glm::vec2 mPos)
{
	if (Collision2D::PointInRect(mPos, createWorld->position, glm::vec2(createWorld->width, createWorld->height)))
		Game::instance->SwitchScene(new CreateWorld());
	else if (Collision2D::PointInRect(mPos, goBack->position, glm::vec2(goBack->width, goBack->height)))
		Game::instance->SwitchScene(new MainMenu());

	// propagate mouse click

	bool clicked = !deleteWorld;

	for (auto o : c2d->objects)
	{
		o->MouseClick(button, mPos);
		if (justDeleted)
			break;
	}

	if (!clicked && deleteWorld) // if it was already true, than that means they didn't click on delete world again)
	{
		for (auto w : worldObjects)
			w->SetDeleteText("Delete");

		deleteWorld = false;
	}
}
