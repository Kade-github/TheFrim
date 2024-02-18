#ifndef _RENDERER_H
#define _RENDERER_H

#pragma once

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

	void Draw(Texture* texture, Shader* shader, Camera* camera, glm::vec3 axis, float angle);

	void AddQuad(glm::vec3 position, glm::vec3 sizeX, float z, glm::vec4 texCoords, glm::vec3 normal, bool flipVert = false)
	{
		glm::vec3 tl = glm::vec3(position.x, position.y + sizeX.y, position.z);
		glm::vec3 tr = glm::vec3(position.x + sizeX.x, position.y + sizeX.y, position.z + sizeX.z);
		glm::vec3 bl = glm::vec3(position.x, position.y, position.z + z);
		glm::vec3 br = glm::vec3(position.x + sizeX.x, position.y, position.z + sizeX.z + z);

		glm::vec2 srcTl = glm::vec2(texCoords.x, texCoords.y);
		glm::vec2 srcBl = glm::vec2(texCoords.x, texCoords.y + texCoords.w);
		glm::vec2 srcTr = glm::vec2(texCoords.x + texCoords.z, texCoords.y);
		glm::vec2 srcBr = glm::vec2(texCoords.x + texCoords.z, texCoords.y + texCoords.w);

		if (flipVert)
		{
			srcTl = glm::vec2(texCoords.x, texCoords.y + texCoords.w);
			srcBl = glm::vec2(texCoords.x, texCoords.y);
			srcTr = glm::vec2(texCoords.x + texCoords.z, texCoords.y + texCoords.w);
			srcBr = glm::vec2(texCoords.x + texCoords.z, texCoords.y);
		}

		AddVertex(tl, normal, srcTl);
		AddVertex(tr, normal, srcTr);
		AddVertex(bl, normal, srcBl);
		AddVertex(bl, normal, srcBl);
		AddVertex(tr, normal, srcTr);
		AddVertex(br, normal, srcBr);

	}

	void AddVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 texCoords)
	{
		Vertex vertex;

		vertex.position = position;
		vertex.normal = normal;
		vertex.texCoords = texCoords;
		vertices.push_back(vertex);
	}
};

#endif