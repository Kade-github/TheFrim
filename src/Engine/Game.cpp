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

	_width = width;
	_height = height;

	log->Write("Window size: " + std::to_string(_width) + "x" + std::to_string(_height));

}

void Game::CreateRenderer()
{
	shader = new Shader();
	shader->LoadShader("Assets/Shaders/vert.glsl", "Assets/Shaders/frag.glsl");

	render = new Renderer();

	render->Init();

	render->SetProjection(75.0f, _width, _height, 0.1f, 100.0f);

	log->Write("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	log->Write("OpenGL renderer: " + std::string((char*)glGetString(GL_RENDERER)));
	log->Write("OpenGL vendor: " + std::string((char*)glGetString(GL_VENDOR)));
	log->Write("OpenGL shading language version: " + std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));
}

void Game::StartGame(Scene* s)
{
	currentScene = s;
}

void Game::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);

	currentScene->Draw();
}
