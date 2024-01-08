/*
    Copyright (c) 2023-2024 Kade

    Please read the LICENSE.md file for more information on licensing.
*/

/* Header guard */
#ifndef _GAME_H_
#define _GAME_H_

#include "../../Includes.h"
#include "Data/Version.h"
#include "../../include/glm/vec2.hpp"

#include "Render/Renderer.h"

namespace Engine
{
	class Game
	{
	private:
		std::string _title;
		Data::Version _version;
		GLFWwindow* _window;

		Render::Renderer* renderer;

	public:
		/// <summary>
		/// Constructor for the game class.
		/// </summary>
		/// <param name="title">- The title of the window</param>
		/// <param name="version">- The version to display</param>
		/// <param name="window">- The GLFW Window Pointer</param>
		Game(std::string title, Data::Version version, GLFWwindow* window);

		/// <summary>
		/// Init the game.
		/// </summary>
		void Init();

		/// <summary>
		/// Sets the current title of the window. (Along with the version set in the constructor)
		/// </summary>
		/// <param name="title">- The title to set the game to</param>
		void SetTitle(std::string title);

		/// <summary>
		/// Set the games 2d size (viewport)
		/// </summary>
		/// <param name="viewport">- The viewport in a 2d vector</param>
		void SetSize(glm::vec2 viewport);

		/// <summary>
		/// Render the screen
		/// </summary>
		void Render();
	};
}

#endif