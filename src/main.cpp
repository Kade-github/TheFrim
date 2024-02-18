#include "Game/TestScene.h"
#include "Engine/Objects/Base/Cube.h"

#include <thread>

int main()
{
	Game game("The Frim", "1.0.0");

	if (!glfwInit())
	{
		game.log->Write("Failed to initialize GLFW");
		exit(EXIT_FAILURE);
	}

	game.log->Write("GLFW initialized");

	glfwSetErrorCallback([](int error, const char* description)
	{ 
		std::cout << "[GLFW] Error: " << description << std::endl;
	});

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	game.CreateWindow(1280,720);

	glfwMakeContextCurrent(game.GetWindow());

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		game.log->Write("Failed to initialize GLAD");
		return -1;
	}

	game.log->Write("GLAD initialized");

	game.CreateRenderer();

	glfwSetFramebufferSizeCallback(game.GetWindow(), [](GLFWwindow* window, int width, int height)
	{
		 Game::instance->SetWindowSize(width, height);
	});


	glfwSetKeyCallback(game.GetWindow(), [](GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GLFW_TRUE);

		if (Game::instance->currentScene != nullptr)
		{
			if (action == GLFW_PRESS)
			{
				switch (key)
				{
				case GLFW_KEY_F4:
					if (Game::instance->isFullscreen)
					{
						glfwSetWindowMonitor(window, NULL, 40, 40, 1280, 720, 0);
						Game::instance->isFullscreen = false;
					}
					else
					{
						GLFWmonitor* monitor = glfwGetPrimaryMonitor();
						const GLFWvidmode* mode = glfwGetVideoMode(monitor);
						glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
						Game::instance->isFullscreen = true;
					}
					break;
				case GLFW_KEY_F1:
					if (Game::instance->lockedCursor)
						glfwSetInputMode(Game::instance->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					else
						glfwSetInputMode(Game::instance->GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

					Game::instance->lockedCursor = !Game::instance->lockedCursor;
					break;
				}

				Game::instance->currentScene->KeyPress(key);
			}
			else if (action == GLFW_RELEASE)
				Game::instance->currentScene->KeyRelease(key);
		}
	});

	glEnable(GL_DEPTH_TEST);

	glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	game.StartGame(new TestScene());


	Texture* sh = Texture::createWithImage("Assets/Textures/sheet.png");

	Cube* cube = new Cube(glm::vec3(1.0f, 1.0f, 1.0f), 2.0f, 2.0f, 2.0f, sh);

	game.currentScene->AddObject(cube);

	Cube* cube2 = new Cube(glm::vec3(-1.0f, 1.0f, 1.0f), 2.0f, 2.0f, 2.0f, sh);

	cube2->rotateAxis = glm::vec3(1.0f, 1.0f, 1.0f);

	game.currentScene->AddObject(cube2);

	glfwMakeContextCurrent(NULL);

	// start render thread

	bool running = true;

	std::thread renderThread([&game, running]()
	{
		glfwMakeContextCurrent(game.GetWindow());
		while (running)
		{
			game.Render();

			glfwSwapBuffers(game.GetWindow());
		}
	});

	renderThread.detach();

	while (running)
	{
		if (glfwWindowShouldClose(game.GetWindow()))
			running = false;
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}