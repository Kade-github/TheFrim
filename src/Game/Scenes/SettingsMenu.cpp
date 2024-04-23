#include "SettingsMenu.h"
#include "MainMenu.h"
#include "../Data/Settings.h"
#include <Helpers/Collision2D.h>
#include "../MusicManager.h"


void SettingsMenu::Create()
{
	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	background = new Sprite2D("Assets/Textures/MainMenu/background.png", glm::vec3(0, 0, 0));

	background->width = c2d->_w;
	background->height = c2d->_h;

	c2d->AddObject(background);

	vsync = new Bar(glm::vec3(0, 0, 0), "VSync: " + std::string(Settings::instance->vsync ? "on" : "off"));

	vsync->position = glm::vec3(c2d->_w / 2, (c2d->_h / 2) + 200, 0);

	vsync->position -= glm::vec3(vsync->width / 2, 0, 0);

	c2d->AddObject(vsync);

	fullscreen = new Bar(glm::vec3(0, 0, 0), "Fullscreen: " + std::string(Settings::instance->fullscreen ? "on" : "off"));

	fullscreen->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 + 100, 0);

	fullscreen->position -= glm::vec3(fullscreen->width / 2, 0, 0);

	c2d->AddObject(fullscreen);

	float fovPerc = Settings::instance->fov / 140;

	if (fovPerc < 0.1f)
		fovPerc = 0.1f;

	if (fovPerc > 1.0f)
		fovPerc = 1.0f;

	fov = new DragBar(glm::vec3(0, 0, 0), "FOV", fovPerc);

	fov->max = 140;

	fov->position = glm::vec3(c2d->_w / 2, c2d->_h / 2, 0);

	fov->position -= glm::vec3(fov->width / 2, 0, 0);

	c2d->AddObject(fov);

	float renderDistancePerc = Settings::instance->renderDistance;

	if (renderDistancePerc < 0.1f)
		renderDistancePerc = 0.1f;

	if (renderDistancePerc > 2.0f)
		renderDistancePerc = 2.0f;

	renderDistance = new DragBar(glm::vec3(0, 0, 0), "Render Distance", renderDistancePerc);

	renderDistance->max = 1;

	renderDistance->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 100, 0);

	renderDistance->position -= glm::vec3(renderDistance->width / 2, 0, 0);

	c2d->AddObject(renderDistance);

	float fogDistancePerc = Settings::instance->fogDistance / 1.2f;

	if (fogDistancePerc < 0.1f)
		fogDistancePerc = 0.1f;

	if (fogDistancePerc > 2.0f)
		fogDistancePerc = 2.0f;

	fogDistance = new DragBar(glm::vec3(0, 0, 0), "Fog Distance", fogDistancePerc);

	fogDistance->max = 1.2;

	fogDistance->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 200, 0);

	fogDistance->position -= glm::vec3(fogDistance->width / 2, 0, 0);

	c2d->AddObject(fogDistance);

	back = new Bar(glm::vec3(0, 0, 0), "Back");

	back->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 300, 0);

	back->position -= glm::vec3(back->width / 2, 0, 0);

	c2d->AddObject(back);
}

void SettingsMenu::Draw()
{
	MusicManager::GetInstance()->Update();

	glm::vec2 mPos = Game::instance->GetCursorPos();

	if (mPos.x > vsync->position.x && mPos.x < vsync->position.x + vsync->width && mPos.y > vsync->position.y && mPos.y < vsync->position.y + vsync->height)
		vsync->selected = true;
	else
		vsync->selected = false;

	if (mPos.x > fullscreen->position.x && mPos.x < fullscreen->position.x + fullscreen->width && mPos.y > fullscreen->position.y && mPos.y < fullscreen->position.y + fullscreen->height)
		fullscreen->selected = true;
	else
		fullscreen->selected = false;

	if (mPos.x > back->position.x && mPos.x < back->position.x + back->width && mPos.y > back->position.y && mPos.y < back->position.y + back->height)
		back->selected = true;
	else
		back->selected = false;

	Settings::instance->fov = fov->value * 140;
	Settings::instance->renderDistance = renderDistance->value;
	Settings::instance->fogDistance = fogDistance->value * 1.2f;

	Scene::Draw();
}

void SettingsMenu::MouseClick(int button, glm::vec2 mPos)
{
	if (button == 0)
	{
		if (vsync->selected)
		{
			Settings::instance->vsync = !Settings::instance->vsync;
			Game::instance->SetVsync(Settings::instance->vsync);
			vsync->SetText("VSync: " + std::string(Settings::instance->vsync ? "on" : "off"));
		}
		else if (fullscreen->selected)
		{
			Settings::instance->fullscreen = !Settings::instance->fullscreen;
			Game::instance->SetFullscreen(Settings::instance->fullscreen);
			fullscreen->SetText("Fullscreen: " + std::string(Settings::instance->fullscreen ? "on" : "off"));
		}
		else if (back->selected)
			Game::instance->SwitchScene(new MainMenu());
	}

	c2d->MouseClick(button, mPos);
}

void SettingsMenu::MouseRelease(int button, glm::vec2 mPos)
{
	c2d->MouseRelease(button, mPos);
}

void SettingsMenu::Resize(float _w, float _h)
{
	if (c2d != nullptr)
		c2d->Resize();
}