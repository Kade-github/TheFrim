#include "Game.h"

Game* Game::instance = nullptr;

Game::Game(std::string title, std::string version)
{
	_title = title;
	_version = version;

	log = new Logging("log.txt");

	instance = this;
}

void Game::CreateWindow(int width, int height)
{
	log->Write("Creating window...");

	_window = glfwCreateWindow(width, height, (_title + " v" + _version).c_str(), NULL, NULL);

	if (!_window)
	{
		log->Write("Failed to create window");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	log->Write("Window created successfully");
}

void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);


}
