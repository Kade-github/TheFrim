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

	vsync->position = glm::vec3(c2d->_w / 2, c2d->_h / 2, 0);

	c2d->AddObject(vsync);

	fullscreen = new Bar(glm::vec3(0, 0, 0), "Fullscreen: " + std::string(Settings::instance->fullscreen ? "on" : "off"));

	fullscreen->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 100, 0);

	c2d->AddObject(fullscreen);

	back = new Bar(glm::vec3(0, 0, 0), "Back");

	back->position = glm::vec3(c2d->_w / 2, c2d->_h / 2 - 200, 0);

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
}

void SettingsMenu::Resize(float _w, float _h)
{
}