#include "Worlds.h"

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

	Sprite2D* deepBackground = new Sprite2D("Assets/Textures/WorldSelect/deep_background.png", glm::vec3(0, 0, 0));
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
}

void Worlds::Resize(float _w, float _h)
{
	c2d->Resize();
}
