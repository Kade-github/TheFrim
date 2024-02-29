#include "Game.h"

Game* Game::instance = nullptr;

float lastFrame = 0.0f; // Time of last frame

Game::Game(std::string title, std::string version)
{
	_title = title;
	_version = version;

	log = new Logging("log.txt");

	instance = this;
	currentScene = nullptr;
}

void Game::CCreateWindow(int width, int height)
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


	glm::mat4 model = glm::mat4(1.0f);

	shader->SetUniformMat4f("model", &model[0][0]);


	log->Write("OpenGL version: " + std::string((char*)glGetString(GL_VERSION)));
	log->Write("OpenGL renderer: " + std::string((char*)glGetString(GL_RENDERER)));
	log->Write("OpenGL vendor: " + std::string((char*)glGetString(GL_VENDOR)));

	_camera = new Camera();
}

void Game::SetScene(Scene* s)
{
	if (currentScene != nullptr)
	{
		currentScene->Destroy();
		delete currentScene;
	}
	currentScene = s;

	currentScene->Create();
	currentScene->isCreated = true;
	currentScene->Resize(_width, _height);
}

void Game::SwitchScene(Scene* s)
{
	toScene = s;
	switchScene = true;
}

void Game::Render()
{
	float currentFrame = glfwGetTime();

	if (swappedScenes)
	{
		lastFrame = currentFrame;
		swappedScenes = false;
	}

	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (switchScene)
	{
		if (currentScene != nullptr)
		{
			currentScene->Destroy();
			delete currentScene;
		}
		currentScene = toScene;
		currentScene->Create();
		currentScene->isCreated = true;
		currentScene->Resize(_width, _height);
		toScene = nullptr;
		switchScene = false;
		swappedScenes = true;
	}

	glClearColor(0.64f, 0.7f, 0.7f, 1.0f);
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

void Game::Destroy()
{
	log->Write("Destroying game...");

	currentScene->Destroy();

	log->Write("Game destroyed successfully");
}
