#ifndef _RENDERER_H
#define _RENDERER_H

#pragma once

#include <vector>
#include "Shader.h"
#include "Model.h"

struct DrawCall
{
	Texture* texture;
	Shader* shader;

	glm::vec3 position;

	Model* m;

	glm::vec3 axis;
	float angle;

	// equals operator

	bool operator==(const DrawCall& other) const
	{
		return texture->id == other.texture->id 
			&& shader->program == other.shader->program 
			&& angle == other.angle 
			&& axis == other.axis;
	}
};


class Renderer {
	unsigned int VBO, VAO, EBO;
public:
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 proj;

	float _width, _height;
	float _far, _near;

	std::vector<DrawCall> drawCalls;

	Renderer()
	{
		Init();
	}

	void SetProjection(float fov, float width, float height, float near, float far)
	{
		_width = width;
		_height = height;
		_near = near;
		_far = far;

		proj = glm::perspective(glm::radians(fov), (float)width / (float)height, near, far);
	}

	void AddDrawCall(glm::vec3 pos, Model* m, Texture* texture, Shader* shader, glm::vec3 axis, float angle)
	{
		DrawCall drawCall;
		drawCall.texture = texture;
		drawCall.shader = shader;
		drawCall.m = m;
		drawCall.position = pos;
		drawCall.axis = axis;
		drawCall.angle = angle;

		drawCalls.push_back(drawCall);
	};

	void Init();

	void Draw(Camera* camera);
};

#endif