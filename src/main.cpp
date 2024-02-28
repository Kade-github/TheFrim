#include "Game/Scenes/MainMenu.h"
#include <thread>

#include <imgui.h>
#include <External/imgui_impl_glfw.h>
#include <External/imgui_impl_opengl3.h>

int main()
{
	Game game("The Frim", "1.0.0");

	if (!glfwInit())
	{
		game.log->Write("Failed to initialize GLFW");
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
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
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);

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
					case GLFW_KEY_F1:
						Game::instance->lockedCursor = !Game::instance->lockedCursor;

						if (Game::instance->lockedCursor)
							glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
						else
							glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
						break;
					}

					Game::instance->currentScene->KeyPress(key);
				}
				else if (action == GLFW_RELEASE)
					Game::instance->currentScene->KeyRelease(key);
			}
		});

	glfwSetMouseButtonCallback(game.GetWindow(), [](GLFWwindow* window, int button, int action, int mods)
		{
			if (Game::instance->currentScene != nullptr && Game::instance->currentScene->isCreated)
			{
				glm::vec2 mPos = Game::instance->GetCursorPos();
				Game::instance->currentScene->MouseClick(button, mPos);
			}
		});

	glfwSetCharCallback(game.GetWindow(), [](GLFWwindow* window, unsigned int codepoint)
		{
			if (Game::instance->currentScene != nullptr && Game::instance->currentScene->isCreated)
				Game::instance->currentScene->OnChar(codepoint);
		});

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);


	glfwMakeContextCurrent(NULL);

	// start render thread

	bool running = true;
	bool threadIsRunning = true;

	std::thread renderThread([&game, &running, &threadIsRunning]()
		{
			glfwMakeContextCurrent(game.GetWindow());

			game.SetScene(new MainMenu());


			while (running)
			{
				game.Render();

				glfwSwapBuffers(game.GetWindow());
			}
			threadIsRunning = false;
		});

	renderThread.detach();

	while (running)
	{
		if (glfwWindowShouldClose(game.GetWindow()))
			running = false;
		glfwWaitEvents();
	}

	Game::instance->log->Write("Shutting down...");

	while (threadIsRunning)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	game.Destroy();

	glfwTerminate();

	return 0;
}