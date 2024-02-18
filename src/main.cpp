#include "Engine/Objects/Base/Blocks/Grass.h"

#include "Game/TestScene.h"

#include <thread>

void APIENTRY glDebugOutput(GLenum source,
	GLenum type,
	unsigned int id,
	GLenum severity,
	GLsizei length,
	const char* message,
	const void* userParam)
{
	char* _source;
	char* _type;
	char* _severity;

	switch (source) {
	case GL_DEBUG_SOURCE_API:
		_source = "API";
		break;

	case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
		_source = "WINDOW SYSTEM";
		break;

	case GL_DEBUG_SOURCE_SHADER_COMPILER:
		_source = "SHADER COMPILER";
		break;

	case GL_DEBUG_SOURCE_THIRD_PARTY:
		_source = "THIRD PARTY";
		break;

	case GL_DEBUG_SOURCE_APPLICATION:
		_source = "APPLICATION";
		break;

	case GL_DEBUG_SOURCE_OTHER:
		_source = "UNKNOWN";
		break;

	default:
		_source = "UNKNOWN";
		break;
	}

	switch (type) {
	case GL_DEBUG_TYPE_ERROR:
		_type = "ERROR";
		break;

	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
		_type = "DEPRECATED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
		_type = "UDEFINED BEHAVIOR";
		break;

	case GL_DEBUG_TYPE_PORTABILITY:
		_type = "PORTABILITY";
		break;

	case GL_DEBUG_TYPE_PERFORMANCE:
		_type = "PERFORMANCE";
		break;

	case GL_DEBUG_TYPE_OTHER:
		_type = "OTHER";
		break;

	case GL_DEBUG_TYPE_MARKER:
		_type = "MARKER";
		break;

	default:
		_type = "UNKNOWN";
		break;
	}

	switch (severity) {
	case GL_DEBUG_SEVERITY_HIGH:
		_severity = "HIGH";
		break;

	case GL_DEBUG_SEVERITY_MEDIUM:
		_severity = "MEDIUM";
		break;

	case GL_DEBUG_SEVERITY_LOW:
		_severity = "LOW";
		break;

	case GL_DEBUG_SEVERITY_NOTIFICATION:
		_severity = "NOTIFICATION";
		break;

	default:
		_severity = "UNKNOWN";
		break;
	}

	if (std::string(_severity) == "NOTIFICATION")
		return;

	Game::instance->log->Write("[OPENGL DEBUG] Source: " + std::string(_source) + " Type: " + std::string(_type) + " Severity: " + std::string(_severity) + " Message: " + std::string(message));
}

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
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

	game.CreateWindow(1280,720);

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

	Model* model = new Model("Assets/Models/cube.obj");

	// create 100 cubes

	for (int i = 0; i < 10; i++)
	{
		for (int j = 0; j < 10; j++)
		{
			glm::vec3 pos = glm::vec3(i * 2, 0, j * 2);

			Grass* cube = new Grass(pos, 2,2,2, model, sh);
			cube->shader = game.shader;

			game.currentScene->AddObject(cube);
		}
	}

	int flags; glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
	if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
	{
		glEnable(GL_DEBUG_OUTPUT);
		glDebugMessageCallback(glDebugOutput, nullptr);
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
	}
	else
	{
		game.log->Write("OpenGL debug context not enabled");
	}

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
		glfwWaitEvents();
	}

	glfwTerminate();

	return 0;
}