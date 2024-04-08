#include "Game/Scenes/MainMenu.h"
#include <thread>

#include <imgui.h>
#include <External/imgui_impl_glfw.h>
#include <External/imgui_impl_opengl3.h>

#include "Game/Data/Settings.h"

int main()
{
	Game game("The Frim", "0.0.3");

	Settings::instance->Load();

	if (!glfwInit())
	{
		game.log->Write("Failed to initialize GLFW");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

#if defined(__linux__)
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
#endif

	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);


	game.CCreateWindow(1920, 1080);

	glfwMakeContextCurrent(game.GetWindow());

	game.log->Write("GLFW initialized");

	glfwSetErrorCallback([](int error, const char* description)
		{
			std::cout << "[GLFW] Error: " << description << std::endl;
		});


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
			if (Game::instance->currentScene != nullptr && Game::instance->currentScene->isCreated)
			{
				if (action == GLFW_PRESS)
				{
					switch (key)
					{
					case GLFW_KEY_F4:
						if (Game::instance->isFullscreen)
						{
							glfwSetWindowMonitor(window, NULL, 40, 40, 1920, 1080, 0);
							Game::instance->isFullscreen = false;
						}
						else
						{
							GLFWmonitor* monitor = glfwGetPrimaryMonitor();
							const GLFWvidmode* mode = glfwGetVideoMode(monitor);
							glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

							Game::instance->GetCamera()->width = mode->width;
							Game::instance->GetCamera()->height = mode->height;

							Game::instance->isFullscreen = true;
						}
						break;
					}

					Game::instance->KeyPress(key);
				}
				else if (action == GLFW_RELEASE)
					Game::instance->KeyRelease(key);
			}
		});

	glfwSetMouseButtonCallback(game.GetWindow(), [](GLFWwindow* window, int button, int action, int mods)
		{
			if (Game::instance->currentScene != nullptr && Game::instance->currentScene->isCreated)
			{
				if (action == GLFW_PRESS)
				{
					glm::vec2 mPos = Game::instance->GetCursorPos();
					Game::instance->MouseClick(button, mPos);
				}
				else
				{
					glm::vec2 mPos = Game::instance->GetCursorPos();
					Game::instance->MouseRelease(button, mPos);
				}
			}
		});

	glfwSetScrollCallback(game.GetWindow(), [](GLFWwindow* window, double xoffset, double yoffset)
		{
			if (Game::instance->currentScene != nullptr && Game::instance->currentScene->isCreated)
			{
				Game::instance->OnScroll(xoffset, yoffset);
			}
		});

	glfwSetCharCallback(game.GetWindow(), [](GLFWwindow* window, unsigned int codepoint)
		{
			if (Game::instance->currentScene != nullptr && Game::instance->currentScene->isCreated)
				Game::instance->OnChar(codepoint);
		});

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	game.lockedCursor = false;


	glfwMakeContextCurrent(NULL);

	// start render thread

	bool running = true;
	bool threadIsRunning = true;

	std::thread renderThread([&game, &running, &threadIsRunning]()
		{
			glfwMakeContextCurrent(game.GetWindow());

			game.SetScene(new MainMenu());

			glfwSetWindowSize(game.GetWindow(), 1920, 1080);
			game.SetWindowSize(1920, 1080);

			while (running)
			{
				game.Render();

				glfwSwapBuffers(game.GetWindow());
			}
			threadIsRunning = false;
		});

	renderThread.detach();

	game.needsUpdate = true;
	game.isFullscreen = Settings::instance->fullscreen;
	game.vsync = Settings::instance->vsync;

	while (running)
	{
		if (glfwWindowShouldClose(game.GetWindow()))
			running = false;
		if (game.needsUpdate)
		{
			if (game.lockedCursor)
				glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			else
				glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			if (game.isFullscreen)
			{
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(game.GetWindow(), monitor, 0, 0, mode->width, mode->height, mode->refreshRate);

				game.GetCamera()->width = mode->width;
				game.GetCamera()->height = mode->height;
			}
			else
			{
				glfwSetWindowMonitor(game.GetWindow(), NULL, 40, 40, 1920, 1080, 0);
				game.GetCamera()->width = 1920;
				game.GetCamera()->height = 1080;
			}

			if (game.vsync)
				glfwSwapInterval(1);
			else
				glfwSwapInterval(0);

			game.needsUpdate = false;
		}
		glfwWaitEventsTimeout(0.01);
	}

	Game::instance->log->Write("Shutting down...");

	while (threadIsRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	Settings::instance->Save();

	game.Destroy();

	glfwTerminate();

	return 0;
}