/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

#include "Includes.h"
#include "Files/Engine/Logging.h"
#include "Files/Engine/Game.h"

/* Main function (or entry point if you want) */

int main(int argc, char* argv[])
{
    Engine::Logging::Init();

    Engine::Logging::Log("Initializing GLFW", "info");

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    Engine::Logging::Log("Creating window", "info");


    Engine::Logging::Log("Creating render thread", "info");

    bool _running = true;

    /* Create render thread */
    GLFWwindow* window = NULL;

    /* Create a windowed mode window */
    window = glfwCreateWindow(1280, 720, "The Frim", NULL, NULL);
    if (!window)
    {
        Engine::Logging::Log("Failed to create window", "error");
        glfwTerminate();
        return -1;
    }


    std::thread th = std::thread([&] {


        /* Create game class */

        Engine::Game game("The Frim", Engine::Data::Version(1, 0, 0), window);

        /* Game resolution */

        game.SetSize(glm::vec2(1280, 720));

        game.Init();

        /* Normally, i'd tell you to lock something that is going to be modified in another thread. But since we don't modify it; it's fair game. */
        while (_running)
        {
            /* Handle rendering here */

            game.Render();
        }
    });

    glfwMakeContextCurrent(NULL);

    Engine::Logging::Log("Starting input thread", "info");

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Handle input here */
        glfwWaitEvents();
    }
    _running = false;

    Engine::Logging::Log("Terminating game", "info");

    Engine::Logging::Destroy();

    glfwDestroyWindow(window);

    glfwTerminate();
    return 0;
}

/* funny windows */

#include <Windows.h>

#ifdef _WIN32
int APIENTRY WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    return main(__argc, __argv);
}
#endif
