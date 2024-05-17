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

	ambientDiffuse = new Bar(glm::vec3(0, 0, 0), "Ambient Diffusion: " + std::string(Settings::instance->useAmbientDiffuse ? "on" : "off"));

	ambientDiffuse->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 + 300, 0);

	ambientDiffuse->position -= glm::vec3(ambientDiffuse->width / 2, 0, 0);

	c2d->AddObject(ambientDiffuse);

	vsync = new Bar(glm::vec3(0, 0, 0), "VSync: " + std::string(Settings::instance->vsync ? "on" : "off"));

	vsync->position = glm::vec3(c2d->_w / 2, (c2d->_h / 2) + 200, 0);

	vsync->position -= glm::vec3(vsync->width / 2, 0, 0);

	c2d->AddObject(vsync);

	fullscreen = new Bar(glm::vec3(0, 0, 0), "Fullscreen: " + std::string(Settings::instance->fullscreen ? "on" : "off"));

	fullscreen->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 + 100, 0);

	fullscreen->position -= glm::vec3(fullscreen->width / 2, 0, 0);

	c2d->AddObject(fullscreen);

	float fovPerc = Settings::instance->fov;

	fov = new DragBar(glm::vec3(0, 0, 0), "FOV", fovPerc);

	fov->max = 140;
	fov->min = 70;

	fov->UpdateBar();

	fov->position = glm::vec3(c2d->_w / 2, c2d->_h / 2, 0);

	fov->position -= glm::vec3(fov->width / 2, 0, 0);

	c2d->AddObject(fov);

	float renderDistancePerc = Settings::instance->renderDistance;

	renderDistance = new DragBar(glm::vec3(0, 0, 0), "Render Distance", renderDistancePerc);

	renderDistance->max = 2.0f;
	renderDistance->min = 0.1f;

	renderDistance->UpdateBar();

	renderDistance->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 100, 0);

	renderDistance->position -= glm::vec3(renderDistance->width / 2, 0, 0);

	c2d->AddObject(renderDistance);

	float fogDistancePerc = Settings::instance->fogDistance;

	fogDistance = new DragBar(glm::vec3(0, 0, 0), "Fog Distance", fogDistancePerc);

	fogDistance->max = 2.0;
	fogDistance->min = 0.1f;

	fogDistance->UpdateBar();

	fogDistance->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 200, 0);

	fogDistance->position -= glm::vec3(fogDistance->width / 2, 0, 0);

	c2d->AddObject(fogDistance);

	masterVolume = new DragBar(glm::vec3(0, 0, 0), "Music Volume", Settings::instance->masterVolume);

	masterVolume->max = 1.0f;
	masterVolume->min = 0.0f;

	masterVolume->UpdateBar();

	masterVolume->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 300, 0);

	masterVolume->position -= glm::vec3(masterVolume->width / 2, 0, 0);

	c2d->AddObject(masterVolume);

	back = new Bar(glm::vec3(0, 0, 0), "Back");

	back->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 400, 0);

	back->position -= glm::vec3(back->width / 2, 0, 0);

	c2d->AddObject(back);
}

void SettingsMenu::Draw()
{
	MusicManager::GetInstance()->Update();

	glm::vec2 mPos = Game::instance->GetCursorPos();

	Settings::instance->masterVolume = masterVolume->value;

	if (mPos.x > vsync->position.x && mPos.x < vsync->position.x + vsync->width && mPos.y > vsync->position.y && mPos.y < vsync->position.y + vsync->height)
		vsync->selected = true;
	else
		vsync->selected = false;

	if (mPos.x > fullscreen->position.x && mPos.x < fullscreen->position.x + fullscreen->width && mPos.y > fullscreen->position.y && mPos.y < fullscreen->position.y + fullscreen->height)
		fullscreen->selected = true;
	else
		fullscreen->selected = false;

	if (mPos.x > ambientDiffuse->position.x && mPos.x < ambientDiffuse->position.x + ambientDiffuse->width && mPos.y > ambientDiffuse->position.y && mPos.y < ambientDiffuse->position.y + ambientDiffuse->height)
		ambientDiffuse->selected = true;
	else
		ambientDiffuse->selected = false;

	if (mPos.x > back->position.x && mPos.x < back->position.x + back->width && mPos.y > back->position.y && mPos.y < back->position.y + back->height)
		back->selected = true;
	else
		back->selected = false;

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
			MusicManager::GetInstance()->PlaySFX("select");
			vsync->SetText("VSync: " + std::string(Settings::instance->vsync ? "on" : "off"));
		}
		else if (fullscreen->selected)
		{
			Settings::instance->fullscreen = !Settings::instance->fullscreen;
			Game::instance->SetFullscreen(Settings::instance->fullscreen);
			MusicManager::GetInstance()->PlaySFX("select");
			fullscreen->SetText("Fullscreen: " + std::string(Settings::instance->fullscreen ? "on" : "off"));
		}
		else if (ambientDiffuse->selected)
		{
			Settings::instance->useAmbientDiffuse = !Settings::instance->useAmbientDiffuse;
			MusicManager::GetInstance()->PlaySFX("select");
			ambientDiffuse->SetText("Ambient Diffusion: " + std::string(Settings::instance->useAmbientDiffuse ? "on" : "off"));
		}
		else if (back->selected)
		{
			MusicManager::GetInstance()->PlaySFX("select");
			Game::instance->SwitchScene(new MainMenu());
		}
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

void SettingsMenu::Destroy()
{
	Settings::instance->fov = fov->value;
	Settings::instance->renderDistance = renderDistance->value;
	Settings::instance->fogDistance = fogDistance->value;


	Settings::instance->Save();
	Scene::Destroy();
}
