/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

#include "Game.h"

#include "Render/OpenGL/OpenGLRenderer.h"

namespace Engine
{
	Game::Game(std::string title, Data::Version version, GLFWwindow* window)
	{
		_version = version;
		_window = window;

		SetTitle(title);

		// Set the renderer

		renderer = new Render::OpenGLRenderer();
	}

	void Game::Init()
	{
		renderer->Init(_window);
	}

	void Game::SetTitle(std::string title)
	{
		_title = title;
		std::string newTitle = _title + " | v" + _version.ToString();
		glfwSetWindowTitle(_window, newTitle.c_str());
		Logging::Log("Set the window title to : " + newTitle + ".", "info");
	}

	void Game::SetSize(glm::vec2 viewport)
	{
		renderer->Viewport = viewport;

		glfwSetWindowSize(_window, viewport.x, viewport.y);
		Logging::Log("Set the viewport size to : " + std::to_string(viewport.x) + "x" + std::to_string(viewport.y) + ".", "info");
	}

	void Game::Render()
	{
		renderer->Render(_window);
	}
}