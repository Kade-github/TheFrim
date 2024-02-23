#include "MainMenu.h"

void MainMenu::Create()
{
	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	background = new Sprite2D("Assets/Textures/MainMenu/background.png", glm::vec3(0, 0, 0));

	c2d->AddObject(background);

	logo = new Sprite2D("Assets/Textures/logo.png", glm::vec3(0, 0, 0));

	c2d->AddObject(logo);
}

void MainMenu::Resize(float _w, float _h)
{
	c2d->Resize();
}
