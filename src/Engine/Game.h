#ifndef _GAME_H
#define _GAME_H

#pragma once
#include <mutex>
#include "Logging.h"
#include "Objects/Scene.h"
#include "../../Include/glad/glad.h"
#include "../../Include/glfw/glfw3.h"
#include "Audio/AudioManager.h"

#include "Objects/Camera.h"
#include "OpenGL/Shader.h"

struct Event {
	int type = 0;
	glm::vec2 pos;
	int var1 = 0;
};

class Game
{
	std::mutex eventMtx;
	GLFWwindow* _window = NULL;
	std::string _version = "";
	std::string _title = "";

	int _width = 0;
	int _height = 0;

	double _cursorX = 0;
	double _cursorY = 0;

	Camera* _camera = NULL;

public:
	static Game* instance;

	AudioManager* audioManager;

	std::vector<Event> events;

	Shader* shader;
	Shader* noFogShader;

	Logging* log;
	bool swappedScenes = false;
	float deltaTime = 0.0f;

	float mouseXP = 1.0f;
	float mouseYP = 1.0f;

	bool vsync = true;
	bool isFullscreen = false;
	bool lockedCursor = true;
	bool needsUpdate = false;
	bool switchScene = false;

	Scene* currentScene;
	Scene* toScene;

	Game(std::string title, std::string version);

	void CCreateWindow(int width, int height);

	void CreateRenderer();

	void SetLockedCursor(bool locked) { lockedCursor = locked; needsUpdate = true; }
	void SetVsync(bool v) { 
		vsync = v; 			
		if (vsync)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);
	}
	void SetFullscreen(bool f) { isFullscreen = f; needsUpdate = true; }

	void SetScene(Scene* s);
	void SwitchScene(Scene* s);

	void CreateScene();

	void Render();

	void Destroy();

	void MouseMove(float x, float y) {
		Event e;
		e.type = 0;
		e.pos = glm::vec2(x, y);
		std::lock_guard<std::mutex> lock(eventMtx);
		events.push_back(e);
	};
	void MouseClick(int button, glm::vec2 mPos) {
		Event e;
		e.type = 1;
		e.pos = mPos;
		e.var1 = button;
		std::lock_guard<std::mutex> lock(eventMtx);
		events.push_back(e);
	};
	void KeyPress(int key) {
		Event e;
		e.type = 2;
		e.var1 = key;
		std::lock_guard<std::mutex> lock(eventMtx);
		events.push_back(e);
	};
	void KeyRelease(int key) {
		Event e;
		e.type = 3;
		e.var1 = key;
		std::lock_guard<std::mutex> lock(eventMtx);
		events.push_back(e);
	};
	void OnChar(unsigned int c) {
		Event e;
		e.type = 4;
		e.var1 = c;
		std::lock_guard<std::mutex> lock(eventMtx);
		events.push_back(e);
	};

	void OnScroll(double x, double y) {
		Event e;
		e.type = 5;
		e.pos = glm::vec2(x, y);
		std::lock_guard<std::mutex> lock(eventMtx);
		events.push_back(e);
	};

	void MouseRelease(int button, glm::vec2 mPos) {
		Event e;
		e.type = 6;
		e.pos = mPos;
		e.var1 = button;
		std::lock_guard<std::mutex> lock(eventMtx);
		events.push_back(e);
	};

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

		shader->Bind();

		shader->SetUniformMat4f("projection", &_camera->GetProjectionMatrix()[0][0]);

		shader->Unbind();

		noFogShader->Bind();

		noFogShader->SetUniformMat4f("projection", &_camera->GetProjectionMatrix()[0][0]);

		noFogShader->Unbind();

		glViewport(0, 0, _width, _height);
	}

	GLFWwindow* GetWindow() { return _window; }

	Camera* GetCamera() { return _camera; }

	glm::vec2 GetWindowSize() { return glm::vec2(_width, _height); }

	glm::vec2 GetCursorPos(bool invert = true) { 

		glfwGetCursorPos(_window, &_cursorX, &_cursorY);

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