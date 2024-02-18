#ifndef _GAME_H
#define _GAME_H

#pragma once

#include "OpenGL/Renderer.h"
#include "Logging.h"
#include "Objects/Scene.h"
#include "../../Include/glfw/glfw3.h"

class Game
{
	GLFWwindow* _window = NULL;
	std::string _version = "";
	std::string _title = "";

	int _width = 0;
	int _height = 0;

	float _cursorX = 0;
	float _cursorY = 0;

	Camera* _camera = NULL;

public:
	static Game* instance;

	Shader* shader;

	Renderer* render;
	Logging* log;

	bool isFullscreen = false;
	bool lockedCursor = true;

	Scene* currentScene;

	Game(std::string title, std::string version);

	void CreateWindow(int width, int height);

	void CreateRenderer();

	void StartGame(Scene* s);

	void Render();


	void SetWindowSize(int width, int height) 
	{ 
		_width = width; _height = height; 
		glViewport(0, 0, _width, _height);
	}

	GLFWwindow* GetWindow() { return _window; }

	Camera* GetCamera() { return _camera; }

	glm::vec2 GetWindowSize() { return glm::vec2(_width, _height); }

	glm::vec2 GetCursorPos() { return glm::vec2(_cursorX, _cursorY); }
};

#endif