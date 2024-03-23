#include "CreateWorld.h"
#include <Game.h>
#include "GeneratingWorld.h"
#include "Worlds.h"
#include <Helpers/Collision2D.h>

#include "../MusicManager.h"

#include <filesystem>

void CreateWorld::Create()
{
	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	Sprite2D* background = new Sprite2D("Assets/Textures/MainMenu/background.png", glm::vec3(0, 0, 0));

	background->width = c2d->_w;
	background->height = c2d->_h;

	c2d->AddObject(background);

	Sprite2D* deepBackground = new Sprite2D("Assets/Textures/WorldSelect/deep_background.png", glm::vec3(0, 0, 0));
	deepBackground->Resize(deepBackground->width * 1.5, deepBackground->height * 1.5);

	deepBackground->position.x = (c2d->_w / 2) - deepBackground->width / 2;
	deepBackground->position.y = ((c2d->_h / 2) + 50) - deepBackground->height / 2;

	c2d->AddObject(deepBackground);

	Text2D* nameHeader = new Text2D("World Name", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 62);

	nameHeader->position.x = deepBackground->position.x + (deepBackground->width / 2) - (nameHeader->width / 2);

	nameHeader->position.y = deepBackground->position.y + ((deepBackground->height - 50) - 62);

	c2d->AddObject(nameHeader);

	name = new InputBar(glm::vec3(0,0,0));

	name->position.x = deepBackground->position.x + (deepBackground->width / 2) - (name->width / 2);
	name->position.y = deepBackground->position.y + ((deepBackground->height - 114) - name->height);

	c2d->AddObject(name);

	Text2D* seedHeader = new Text2D("Seed", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 62);

	seedHeader->position.x = deepBackground->position.x + (deepBackground->width / 2) - (seedHeader->width / 2);

	seedHeader->position.y = name->position.y - 114;

	c2d->AddObject(seedHeader);

	seed = new InputBar(glm::vec3(0, 0, 0));

	seed->position.x = deepBackground->position.x + (deepBackground->width / 2) - (seed->width / 2);

	seed->position.y = seedHeader->position.y - 70;

	seed->maxChars = 20;

	c2d->AddObject(seed);

	createWorld = new Bar(glm::vec3(0, 0, 0), "Generate World");
	createWorld->Resize((createWorld->width * 0.8) - 100, createWorld->height * 0.8);

	createWorld->position.x = deepBackground->position.x + (deepBackground->width / 2) - (createWorld->width / 2);
	createWorld->position.y = seed->position.y - 114;

	c2d->AddObject(createWorld);

	goBack = new Bar(glm::vec3(0, 0, 0), "Go Back");
	goBack->Resize((goBack->width * 0.8) - 100, goBack->height * 0.8);

	goBack->position.x = (c2d->_w / 2) - (goBack->width / 2);
	goBack->position.y = 50;

	c2d->AddObject(goBack);
}

void CreateWorld::Resize(float _w, float _h)
{
	c2d->Resize();
}

void CreateWorld::Draw()
{
	MusicManager::GetInstance()->Update();

	Scene::Draw();
}

void CreateWorld::MouseClick(int button, glm::vec2 mPos)
{
	if (Collision2D::PointInRect(mPos, name->position, glm::vec2(name->width, name->height)))
		name->selected = true;
	else
		name->selected = false;

	if (Collision2D::PointInRect(mPos, seed->position, glm::vec2(seed->width, seed->height)))
		seed->selected = true;
	else
		seed->selected = false;

	if (Collision2D::PointInRect(mPos, createWorld->position, glm::vec2(createWorld->width, createWorld->height)))
	{
		if (name->text.length() > 0)
		{
			// check if the world already exists

			if (std::filesystem::exists("worlds/" + name->text))
				return;

			MusicManager::GetInstance()->FadeOut(4);
			MusicManager::GetInstance()->PlaySFX("select");
			Game::instance->SwitchScene(new GeneratingWorld(name->text, seed->text));
		}
	}

	if (Collision2D::PointInRect(mPos, goBack->position, glm::vec2(goBack->width, goBack->height)))
	{
		MusicManager::GetInstance()->PlaySFX("select");
		Game::instance->SwitchScene(new Worlds());
	}
}

void CreateWorld::OnChar(unsigned int c)
{
	if (name->selected)
		name->AddText(c);

	if (seed->selected)
		seed->AddText(c);
}

void CreateWorld::KeyPress(int key)
{
	if (key == GLFW_KEY_BACKSPACE)
	{
		if (name->selected)
			name->RemoveText();

		if (seed->selected)
			seed->RemoveText();
	}
}
