#ifndef _RENDER_H
#define _RENDER_H

#pragma once

#include "../../Include/glm/glm.hpp"
#include "Texture.h"
#include "Shader.h"
#include "../Objects/Camera.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;
};

class Renderer {
	unsigned int VBO, VAO;
public:
	glm::mat4 model = glm::mat4(1.0f);
	glm::mat4 proj;

	float _width, _height;
	float _far, _near;

	std::vector<Vertex> vertices;

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

	void Init();

	void Draw(Texture* texture, Shader* shader, Camera* camera);

	void AddQuad(glm::vec3 position, glm::vec3 sizeX, float z, glm::vec2 texCoords)
	{
		// 2 triangles (with size x, y, z)

		glm::vec3 tl = glm::vec3(position.x, position.y + sizeX.y, position.z);
		glm::vec3 tr = glm::vec3(position.x + sizeX.x, position.y + sizeX.y, position.z + sizeX.z);
		glm::vec3 bl = glm::vec3(position.x, position.y, position.z + z);
		glm::vec3 br = glm::vec3(position.x + sizeX.x, position.y, position.z + sizeX.z + z);

		AddVertex(tl, glm::vec2(texCoords.x, texCoords.y + 1));
		AddVertex(bl, glm::vec2(texCoords.x, texCoords.y));
		AddVertex(tr, glm::vec2(texCoords.x + 1, texCoords.y + 1));
		AddVertex(tr, glm::vec2(texCoords.x + 1, texCoords.y + 1));
		AddVertex(bl, glm::vec2(texCoords.x, texCoords.y));
		AddVertex(br, glm::vec2(texCoords.x + 1, texCoords.y));
	}

	void AddVertex(glm::vec3 position, glm::vec2 texCoords)
	{
		Vertex vertex;

		glm::vec3 normal = glm::vec3(0.0f, 0.0f, 0.0f);

		vertex.position = position;
		vertex.normal = normal;
		vertex.texCoords = texCoords;
		vertices.push_back(vertex);
	}
};

#endif