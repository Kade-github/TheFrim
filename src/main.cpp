#include "Engine/Game.h"

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

	glfwSetFramebufferSizeCallback(game.GetWindow(), [](GLFWwindow* window, int width, int height)
	{
		 Game::instance->SetWindowSize(width, height);
	});

	glfwSetCursorPosCallback(game.GetWindow(), [](GLFWwindow* window, double xpos, double ypos)
	{
		Game::instance->SetCursorPos(xpos, ypos);
	});

	glEnable(GL_DEPTH_TEST);

	while (!glfwWindowShouldClose(game.GetWindow()))
	{
		game.Render();

		glfwSwapBuffers(game.GetWindow());
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}