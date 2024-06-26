
#include "Game/Scenes/MainMenu.h"
#include <thread>

#include <stbi.h>

#include "Game/Data/Settings.h"


int main()
{
	Game game("The Frim", "1.0.0");

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

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);


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

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

	glfwSetInputMode(game.GetWindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	if (glfwRawMouseMotionSupported())
		glfwSetInputMode(game.GetWindow(), GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

	game.lockedCursor = false;

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

	glfwSetWindowFocusCallback(game.GetWindow(), [](GLFWwindow* window, int focused)
		{
			Game::instance->FocusChange(focused);
		});

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


	GLFWimage images[1];

	images[0].pixels = stbi_h::stbi_load_file_data("Assets/Icon/frim_icon.png", &images[0].width, &images[0].height);

	glfwSetWindowIcon(game.GetWindow(), 1, images);

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

				game.SetWindowSize(mode->width, mode->height);
			}
			else
			{
				glfwSetWindowMonitor(game.GetWindow(), NULL, 40, 40, 1920, 1080, 0);
				game.SetWindowSize(1920, 1080);
			}

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