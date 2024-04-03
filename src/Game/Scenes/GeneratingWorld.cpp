#include "GeneratingWorld.h"
#include "Gameplay.h"
#include <Game.h>
#include <Helpers/StringTools.h>
#include <Objects/2DSprite.h>
#include "../MusicManager.h"

GeneratingWorld::GeneratingWorld(std::string name, std::string seed)
{
	_name = name;
	_seed = seed;
}

void GeneratingWorld::Create()
{
	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	Sprite2D* background = new Sprite2D("Assets/Textures/MainMenu/background.png", glm::vec3(0, 0, 0));

	background->width = c2d->_w;
	background->height = c2d->_h;

	c2d->AddObject(background);

	Texture* t = Texture::createWithImage("Assets/Textures/block.png");

	t->spriteSheet.Load("Assets/Textures/block.xml", t->width, t->height);

	t->dontDelete = true;

	wm = new WorldManager(_name, t, _seed);

	label = new Text2D("Generating World... (0%)", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 62);

	label->position.x = (c2d->_w / 2);
	label->position.y = (c2d->_h / 2);

	label->center = true;

	c2d->AddObject(label);
}

void GeneratingWorld::Resize(float _w, float _h)
{
	c2d->Resize();
}

void GeneratingWorld::Draw()
{
	static bool switched = false;

	// update regions

	wm->CheckGeneratedRegions();

	MusicManager::GetInstance()->Update();

	int regions = wm->regions.size();

	float progress = (float)regions / 9.0f;

	label->text = "Generating World... (" + StringTools::ToTheDecimial(progress * 100, 2) + "%)";

	if (progress >= 1 && !switched)
	{
		switched = true;
		Game::instance->SwitchScene(new Gameplay(wm));
	}

	Scene::Draw();
}