#include "Game.h"

Game* Game::instance = nullptr;

Game::Game(std::string title, std::string version)
{
	_title = title;
	_version = version;

	log = new Logging("log.txt");

	instance = this;
	currentScene = nullptr;
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

	shader->Bind();

	log->Write("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	log->Write("OpenGL renderer: " + std::string((char*)glGetString(GL_RENDERER)));
	log->Write("OpenGL vendor: " + std::string((char*)glGetString(GL_VENDOR)));
	log->Write("OpenGL shading language version: " + std::string((char*)glGetString(GL_SHADING_LANGUAGE_VERSION)));

	_camera = new Camera();
}

void Game::SetScene(Scene* s)
{
	if (currentScene != nullptr)
		delete currentScene;
	currentScene = s;

	currentScene->Create();
}

void Game::Render()
{
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, _camera->width, _camera->height);

	// set mouse

	if (lockedCursor)
	{
		double mX, mY;

		glfwGetCursorPos(_window, &mX, &mY);

		_cursorX = mX;
		_cursorY = mY;
	}


	currentScene->Draw();
}
