#include "Worlds.h"
#include "MainMenu.h"
#include "CreateWorld.h"
#include "LoadingWorld.h"
#include "../MusicManager.h"
#include <filesystem>
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

	scrollBar = new Rectangle2D(glm::vec3(0, 0, 0), glm::vec2(20, 100), glm::vec4(0.125f, 0.125f, 0.125f, 0));

	c2d->AddObject(scrollBar);

	CreateWorldObjects();
}

void Worlds::Resize(float _w, float _h)
{
	if (c2d != nullptr)
		c2d->Resize();
}

void Worlds::SetScroll()
{
	if (worldObjects.size() == 0)
		return;

	scrollBar->height = ((worldObjects[0]->height * 4) / worlds.size()) + 32;

	float inital = deepBackground->position.y + deepBackground->height - (scrollBar->height + 32);

	scrollBar->position.y = inital;

	scrollBar->position.y -= scrollBar->height * (scrollModifier / 100);

	if (scrollBar->position.y < deepBackground->position.y + 32)
	{
		scrollBar->position.y = deepBackground->position.y + 32;
		scrollModifier = _lastScroll;
	}
	else
		_lastScroll = scrollModifier;
	
}

void Worlds::CreateWorldObjects()
{
	for (auto w : worldObjects)
	{
		c2d->RemoveObject(w);
		delete w;
	}

	worldObjects.clear();

	canScroll = false;

	for (int i = 0; i < worlds.size(); i++)
	{
		Data::World w = worlds[i];

		World* world = new World(glm::vec3(0, 0, 0));

		if (std::filesystem::exists(w._path + "/screenshot.png"))
			world->SetThumbnail(w._path + "/screenshot.png");

		world->clip	= glm::vec4(deepBackground->position.x + 10, deepBackground->position.y + 20, deepBackground->width - 20, deepBackground->height - 40);

		world->width *= 1.45;
		world->height *= 1.45;

		world->SetName(w.name);
		world->SetSeed(w.seed);

		world->position.x = deepBackground->position.x + (deepBackground->width / 2) - (world->width / 2);
		world->position.y = deepBackground->position.y + deepBackground->height - (world->height + 64) - ((world->height + 24) * i);

		if (world->position.y + world->height > deepBackground->position.y + deepBackground->height || world->position.y < deepBackground->position.y)
			canScroll = true;

		world->SetSelectCallback([&, i]() {
			MusicManager::GetInstance()->FadeOut(4);
			MusicManager::GetInstance()->PlaySFX("select");
			LoadWorld(worlds[i]);
		});

		world->SetDeleteCallback([&, world, i]() {
			if (!deleteWorld)
			{
				deleteWorld = true;
				// give warning
				MusicManager::GetInstance()->PlaySFX("select");
				world->SetDeleteText("Really?");
			}
			else
			{
				MusicManager::GetInstance()->PlaySFX("select");
				DeleteWorld(worlds[i]);
				deleteWorld = false;
			}
		});

		c2d->AddObject(world);

		worldObjects.push_back(world);
	}

	if (canScroll)
	{
		scrollModifier = 0;
		_lastScroll = 0.0f;
		scrollBar->position.x = deepBackground->position.x + deepBackground->width - 60;
		SetScroll();
		scrollBar->color.w = 0.75;
	}
	else
	{
		scrollModifier = 0;
		_lastScroll = 0.0f;
		scrollBar->color.w = 0;
	}
}

void Worlds::LoadWorld(Data::World w)
{
	Game::instance->SwitchScene(new LoadingWorld(w.name));
}

void Worlds::DeleteWorld(Data::World w)
{
	WorldManager::instance->DeleteWorld(w);
	justDeleted = true;
}

void Worlds::Draw()
{
	MusicManager::GetInstance()->Update();

	for (int i = 0; i < worldObjects.size(); i++)
	{
		World* w = worldObjects[i];

		w->position.y = deepBackground->position.y + deepBackground->height - (w->height + 64) - ((w->height + 24) * i);
		w->position.y += (w->height * 6) * (scrollModifier / 100);
	}

	if (justDeleted)
	{
		worlds = WorldManager::GetWorlds();
		CreateWorldObjects();
		justDeleted = false;

	}

	Scene::Draw();
}

void Worlds::MouseClick(int button, glm::vec2 mPos)
{
	if (Collision2D::PointInRect(mPos, createWorld->position, glm::vec2(createWorld->width, createWorld->height)))
	{
		MusicManager::GetInstance()->PlaySFX("select");
		Game::instance->SwitchScene(new CreateWorld());
	}
	else if (Collision2D::PointInRect(mPos, goBack->position, glm::vec2(goBack->width, goBack->height)))
	{
		MusicManager::GetInstance()->PlaySFX("select");
		Game::instance->SwitchScene(new MainMenu());
	}

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

void Worlds::OnScroll(double x, double y)
{
	if (!canScroll)
		return;

	scrollModifier -= y * 6.0f;

	if (scrollModifier < 0)
		scrollModifier = 0;

	SetScroll();
}

void Worlds::Destroy() {
    for (auto w : worldObjects)
    {
        c2d->RemoveObject(w);
        delete w;
    }

    worldObjects.clear();

    Scene::Destroy();
}
