#ifndef _GAMEOBJECT_H
#define _GAMEOBJECT_H

#pragma once

#include <string>

#include "../../../Include/glm/gtc/type_ptr.hpp"
#include "../../../Include/glm/gtc/matrix_transform.hpp"

class GameObject
{
public:
	struct VVertex {
		glm::vec3 position;
		glm::vec2 uv;
		glm::vec3 normal;

		VVertex(glm::vec3 _position, glm::vec2 _uv, glm::vec3 _normal)
		{
			position = _position;
			uv = _uv;
			normal = _normal;
		}

		VVertex(glm::vec3 _position, glm::vec2 _uv)
		{
			position = _position;
			uv = _uv;
			normal = glm::vec3(0,0,0);
		}

		VVertex(glm::vec3 _position)
		{
			position = _position;
			uv = glm::vec2(0,0);
			normal = glm::vec3(0,0,0);
		}

		VVertex()
		{
			position = glm::vec3(0, 0, 0);
			uv = glm::vec2(0, 0);
			normal = glm::vec3(0,0,0);
		}
	};

	GameObject(glm::vec3 pos)
	{
		position = pos;
	}

	bool visible = true;
	bool delayed = false;

	std::string tag_id = "object";

	int order = 0;
	int id = -1;

	glm::vec3 position = {};

	glm::vec3 rotateAxis = glm::vec3(0.5, 0.5, 0.5);
	float angle = 0;

	virtual void Draw() {};

	virtual void Create() {};

	virtual void Destroy() {};

	virtual void MouseMove(float x, float y) {};
	virtual void MouseClick(int button, glm::vec2 mPos) {};
	virtual void MouseRelease(int button, glm::vec2 pos) {};
	virtual void KeyPress(int key) {};
	virtual void KeyRelease(int key) {};
	virtual void OnChar(unsigned int c) {};
	virtual void OnScroll(double x, double y) {};
};

#endif