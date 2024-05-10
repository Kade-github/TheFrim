#include "MainMenu.h"
#include "Worlds.h"
#include "SettingsMenu.h"
#include <cmath>
#include <Helpers/Collision2D.h>
#include "../MusicManager.h"
#include "../CraftingManager.h"

void MainMenu::Create()
{
	CraftingManager::GetInstance()->Init();

	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	background = new Sprite2D("Assets/Textures/MainMenu/background.png", glm::vec3(0, 0, 0));

	background->width = c2d->_w;
	background->height = c2d->_h;

	c2d->AddObject(background);

	logo = new Sprite2D("Assets/Textures/logo.png", glm::vec3(0, 0, 0));

	c2d->AddObject(logo);

	selectWorld = new Bar(glm::vec3(0, 0, 0), "Play");
	selectWorld->Resize((selectWorld->width * 1.5) - 100, selectWorld->height * 1.5);

	selectWorld->position.x = c2d->_w - (selectWorld->width);
	selectWorld->position.y = c2d->_h - 250;

	c2d->AddObject(selectWorld);

	settings = new Bar(glm::vec3(0, 0, 0), "Settings");
	settings->Resize((settings->width * 1.5) - 100, settings->height * 1.5);

	settings->position.x = c2d->_w - (settings->width + 100);
	settings->position.y = c2d->_h - 450;

	c2d->AddObject(settings);

	exit = new Bar(glm::vec3(0, 0, 0), "Exit");
	exit->Resize((exit->width * 1.5) - 100, exit->height * 1.5);

	exit->position.x = c2d->_w - (exit->width + 100);
	exit->position.y = c2d->_h - 650;

	c2d->AddObject(exit);

	Game::instance->SetLockedCursor(false);
}

void MainMenu::Draw()
{
	if (std::floor(fakeIndex) != selectedIndex)
		fakeIndex = std::lerp(fakeIndex, selectedIndex, 0.1f);

	MusicManager::GetInstance()->Update();

	glm::vec2 mPos = Game::instance->GetCursorPos();

	switch (selectedIndex)
	{
		case 0:
			selectWorld->position.x = std::lerp(selectWorld->position.x, c2d->_w - (selectWorld->width), 9.0f * Game::instance->deltaTime);
			settings->position.x = std::lerp(settings->position.x, c2d->_w - (settings->width - 100),9.0f * Game::instance->deltaTime);
			exit->position.x = std::lerp(exit->position.x, c2d->_w - (exit->width - 100), 9.0f * Game::instance->deltaTime);
			break;
		case 1:
			selectWorld->position.x = std::lerp(selectWorld->position.x, c2d->_w - (selectWorld->width - 100), 9.0f * Game::instance->deltaTime);
			settings->position.x = std::lerp(settings->position.x, c2d->_w - (settings->width), 9.0f * Game::instance->deltaTime);
			exit->position.x = std::lerp(exit->position.x, c2d->_w - (exit->width - 100), 9.0f * Game::instance->deltaTime);
			break;
		case 2:
			selectWorld->position.x = std::lerp(selectWorld->position.x, c2d->_w - (selectWorld->width - 100), 9.0f * Game::instance->deltaTime);
			settings->position.x = std::lerp(settings->position.x, c2d->_w - (settings->width - 100),9.0f * Game::instance->deltaTime);
			exit->position.x = std::lerp(exit->position.x, c2d->_w - (exit->width), 9.0f * Game::instance->deltaTime);
			break;
		case -1:
			selectWorld->position.x = std::lerp(selectWorld->position.x, c2d->_w - (selectWorld->width - 100), 9.0f * Game::instance->deltaTime);
			settings->position.x = std::lerp(settings->position.x, c2d->_w - (settings->width - 100), 9.0f * Game::instance->deltaTime);
			exit->position.x = std::lerp(exit->position.x, c2d->_w - (exit->width - 100), 9.0f * Game::instance->deltaTime);
			break;
	}

	if (mPos.y > 0 && mPos.x > 0 && mPos.x < c2d->_w && mPos.y < c2d->_h)
	{
		if (Collision2D::PointInRect(mPos, selectWorld->position, glm::vec2{ selectWorld->width, selectWorld->height }))
		{
			selectedIndex = 0;
			selectWorld->selected = true;
			settings->selected = false;
			exit->selected = false;
			mouseSelected = true;
		}
		else if (Collision2D::PointInRect(mPos, settings->position, glm::vec2{ settings->width, settings->height }))
		{
			selectedIndex = 1;
			selectWorld->selected = false;
			settings->selected = true;
			exit->selected = false;
			mouseSelected = true;
		}
		else if (Collision2D::PointInRect(mPos, exit->position, glm::vec2{ exit->width, exit->height }))
		{
			selectedIndex = 2;
			selectWorld->selected = false;
			settings->selected = false;
			exit->selected = true;
			mouseSelected = true;
		}
		else if (mouseSelected)
		{
			selectedIndex = -1;
			selectWorld->selected = false;
			settings->selected = false;
			exit->selected = false;
		}
	}


	Scene::Draw();
}

void MainMenu::MouseClick(int button, glm::vec2 mPos)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		if (selectWorld->selected)
		{
			MusicManager::GetInstance()->PlaySFX("select");
			Game::instance->SwitchScene(new Worlds());
		}
		else if (settings->selected)
		{
			MusicManager::GetInstance()->PlaySFX("select");
			Game::instance->SwitchScene(new SettingsMenu());
		}
		else if (exit->selected)
			glfwSetWindowShouldClose(Game::instance->GetWindow(), true);
	}
}

void MainMenu::Resize(float _w, float _h)
{
	if (c2d != nullptr)
		c2d->Resize();
}

void MainMenu::KeyPress(int key)
{
	c2d->KeyPress(key);
}
