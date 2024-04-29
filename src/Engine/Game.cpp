#include "Game.h"
#include "stbi.h"
#include "TextureCache.h"

Game* Game::instance = nullptr;

float lastFrame = 0.0f; // Time of last frame

void Game::CaptureScreen()
{
	int width, height;
	glfwGetFramebufferSize(_window, &width, &height);

	unsigned char* data = new unsigned char[width * height * 3];

	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);

	unsigned char* data2 = new unsigned char[196609];

	stbi_h::stbi_resize(data, width, height, 3, 256, 256, data2);

	stbi_h::stbi_save_png(_screenshotPath.c_str(), 256, 256, 3, data2, 0);

	delete[] data;
    delete[] data2;
}

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

void Game::SetWindowSize(int width, int height)
{
	_width = width;
	_height = height;

	mouseXP = _width / 1920.0f;
	mouseYP = _height / 1080.0f;

	log->Write("Width: " + std::to_string(_width) + ". Height: " + std::to_string(_height));

	log->Write("Set mouse percentages to " + std::to_string(mouseXP) + "," + std::to_string(mouseYP));

	if (currentScene == nullptr)
		return;

	currentScene->Resize(_width, _height);

	glm::mat4 project = _camera->GetProjectionMatrix();

	shader->Bind();

	shader->SetUniformMat4f("projection", &project[0][0]);

	shader->Unbind();


	glViewport(0, 0, _width, _height);
}


void Game::CreateRenderer()
{
	audioManager = new AudioManager();
	log->Write("Audio manager created...");

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

void Game::TakeScreenshot(std::string path)
{
	_takeScreenshot = true;
	_screenshotPath = path;
}

void Game::SetScene(Scene* s)
{
	if (currentScene != nullptr)
	{
		currentScene->Destroy();
		delete currentScene;
	}
	currentScene = s;

	CreateScene();
}

void Game::SwitchScene(Scene* s)
{
	toScene = s;
	switchScene = true;
}

void Game::CreateScene()
{
	TextureCache::Clear();

	currentScene->Create();
	currentScene->isCreated = true;
}

void Game::Render()
{
	float currentFrame = glfwGetTime();

	if (events.size() > 0)
	{
		std::lock_guard<std::mutex> lock(eventMtx);
		for (auto& e : events)
		{
			switch (e.type)
			{
				case 0:
					currentScene->MouseMove(e.pos.x, e.pos.y);
					break;
				case 1:
					currentScene->MouseClick(e.var1, e.pos);
					break;
				case 2:
					currentScene->KeyPress(e.var1);
					break;
				case 3:
					currentScene->KeyRelease(e.var1);
					break;
				case 4:
					currentScene->OnChar(e.var1);
					break;
				case 5:
					currentScene->OnScroll(e.pos.x, e.pos.y);
					break;
				case 6:
					currentScene->MouseRelease(e.var1, e.pos);
					break;
				case 7:
					currentScene->FocusChange(e.var1);
					break;
			}
		}

		events.clear();
	}

	if (swappedScenes)
	{
		lastFrame = currentFrame;
		swappedScenes = false;
	}

	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	if (_takeScreenshot)
	{
		CaptureScreen();
		_tookScreenshot = true;
		_takeScreenshot = false;
	}

	if (switchScene && (!_tookScreenshot && !_takeScreenshot))
	{
		if (currentScene != nullptr)
		{
			currentScene->Destroy();
			delete currentScene;
		}
		currentScene = toScene;
		CreateScene();
		toScene = nullptr;
		switchScene = false;
		swappedScenes = true;
	}

	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
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
