#ifndef _GAME_H
#define _GAME_H

#pragma once

#include "Logging.h"
#include "../../Include/glfw/glfw3.h"
#include "../../Include/glad/glad.h"
#include "../../Include/glm/vec2.hpp"

class Game
{
	GLFWwindow* _window = NULL;
	std::string _version = "";
	std::string _title = "";

	int _width = 0;
	int _height = 0;

	float _cursorX = 0;
	float _cursorY = 0;
public:
	static Game* instance;
	Logging* log;

	Game(std::string title, std::string version);

	void CreateWindow(int width, int height);

	void Render();

	void SetCursorPos(float x, float y) { _cursorX = x; _cursorY = y; }

	void SetWindowSize(int width, int height) 
	{ 
		_width = width; _height = height; 
		glViewport(0, 0, _width, _height);
	}

	GLFWwindow* GetWindow() { return _window; }

	glm::vec2 GetWindowSize() { return glm::vec2(_width, _height); }

	glm::vec2 GetCursorPos() { return glm::vec2(_cursorX, _cursorY); }
};

#endif