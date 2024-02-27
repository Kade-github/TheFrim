#ifndef _GAME_H
#define _GAME_H

#pragma once

#include "Logging.h"
#include "Objects/Scene.h"
#include "../../Include/glad/glad.h"
#include "../../Include/glfw/glfw3.h"

#include "Objects/Camera.h"
#include "OpenGL/Shader.h"

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

	Logging* log;

	float deltaTime = 0.0f;

	float mouseXP = 1.0f;
	float mouseYP = 1.0f;

	bool isFullscreen = false;
	bool lockedCursor = true;
	bool switchScene = false;

	Scene* currentScene;
	Scene* toScene;

	Game(std::string title, std::string version);

	void CCreateWindow(int width, int height);

	void CreateRenderer();

	void SetScene(Scene* s);
	void SwitchScene(Scene* s);

	void Render();

	void Destroy();

	void SetWindowSize(int width, int height) 
	{ 
		_width = width; 
		_height = height; 

		mouseXP = _width / 1920.0f;
		mouseYP = _height / 1080.0f;

		log->Write("Width: " + std::to_string(_width) + ". Height: " + std::to_string(_height));

		log->Write("Set mouse percentages to " + std::to_string(mouseXP) + "," + std::to_string(mouseYP));

		_camera->width = width;
		_camera->height = height;

		currentScene->Resize(width, height);

		glViewport(0, 0, _width, _height);
	}

	void CaptureCursor(bool capture)
	{
		if (capture)
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

	GLFWwindow* GetWindow() { return _window; }

	Camera* GetCamera() { return _camera; }

	glm::vec2 GetWindowSize() { return glm::vec2(_width, _height); }

	glm::vec2 GetCursorPos(bool invert = true) { 
		float y = _cursorY;

		if (invert) // invert y axis
			y = _height - _cursorY;

		float x = _cursorX;

		x /= mouseXP;
		y /= mouseYP;

		return glm::vec2(x, y); 
	}
};

#endif