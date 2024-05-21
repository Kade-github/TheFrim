#ifndef _CAMERA_H
#define _CAMERA_H

#include <vector>
#include "GameObject.h"
#include "../OpenGL/Texture.h"

#pragma once

#include "../../../Include/glm/gtc/type_ptr.hpp"
#include "../../../Include/glm/gtc/matrix_transform.hpp"

class Camera
{
	std::vector<GameObject::VVertex> CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv);
	void ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal);
	Texture* debug = nullptr;
public:
	unsigned int VAO, VBO;

	std::vector<GameObject::VVertex> vertices;

	float width = 1920;
	float height = 1080;

	float cameraFar = 100.0f;

	glm::vec3 smoothFront = glm::vec3(0.0f, 0.0f, -1.0f);

	glm::vec3 scale = glm::vec3(1, 1, 1);

	Camera();

	~Camera();

	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw = 0;
	float pitch = 0;

	float fov = 75.0f;

	void DrawDebugCube(glm::vec3 pos, glm::vec3 scale = glm::vec3(1,1,1));

	void DebugDraws();

	void LookAt(glm::vec3 target)
	{
		glm::vec3 direction = -glm::normalize(position - target);
		pitch = glm::degrees(asin(direction.y));
		yaw = glm::degrees(atan2(direction.z, direction.x));
	}

	glm::mat4 calculate_lookAt_matrix(glm::vec3 position, glm::vec3 target, glm::vec3 worldUp)
	{
		// 1. Position = known
		// 2. Calculate cameraDirection
		glm::vec3 zaxis = glm::normalize(position - target);
		// 3. Get positive right axis vector
		glm::vec3 xaxis = glm::normalize(glm::cross(glm::normalize(worldUp), zaxis));
		// 4. Calculate camera up vector
		glm::vec3 yaxis = glm::cross(zaxis, xaxis);

		// Create translation and rotation matrix
		// In glm we access elements as mat[col][row] due to column-major layout
		glm::mat4 translation = glm::mat4(1.0f); // Identity matrix by default
		translation[3][0] = -position.x; // Fourth column, first row
		translation[3][1] = -position.y;
		translation[3][2] = -position.z;
		glm::mat4 rotation = glm::mat4(1.0f);
		rotation[0][0] = xaxis.x; // First column, first row
		rotation[1][0] = xaxis.y;
		rotation[2][0] = xaxis.z;
		rotation[0][1] = yaxis.x; // First column, second row
		rotation[1][1] = yaxis.y;
		rotation[2][1] = yaxis.z;
		rotation[0][2] = zaxis.x; // First column, third row
		rotation[1][2] = zaxis.y;
		rotation[2][2] = zaxis.z;

		// Return lookAt matrix as combination of translation and rotation matrix
		return rotation * translation; // Remember to read from right to left (first translation then rotation)
	}


	float YawAngleTo(glm::vec3 pos)
	{
		float angle = glm::degrees(atan2(pos.z - position.z, pos.x - position.x)) - yaw;

		angle += 90; // center the angle

		angle = fmod(angle, 360.0f);

		if (angle < 0)
			angle += 360.0f;

		return angle;
	}

	void SetDirectionSmooth(float amount);

	void SetDirection();
	void SetViewMatrix();

	glm::vec2 WorldToScreen(glm::vec3 worldPos)
	{
		glm::mat4 view = GetViewMatrix();
		glm::mat4 projection = GetProjectionMatrix();

		glm::vec4 clipCoords = projection * glm::vec4(worldPos, 1.0f);
		clipCoords /= clipCoords.w;

		glm::vec4 ndc = glm::inverse(projection) * clipCoords;
		ndc.z = -1.0f;
		ndc.w = 0.0f;

		glm::vec4 viewPos = glm::inverse(view) * ndc;
		viewPos = glm::vec4(viewPos.x, viewPos.y, -viewPos.z, 1.0f);

		glm::vec2 screenPos;
		screenPos.x = (viewPos.x + 1.0f) * width / 2.0f;
		screenPos.y = (1.0f - viewPos.y) * height / 2.0f;

		return screenPos;
	}

	glm::vec3 ScreenToWorld(glm::vec2 screenPos)
	{
		glm::mat4 view = GetViewMatrix();
		glm::mat4 projection = GetProjectionMatrix();

		glm::vec4 clipCoords;
		clipCoords.x = (2.0f * screenPos.x) / width - 1.0f;
		clipCoords.y = 1.0f - (2.0f * screenPos.y) / height;
		clipCoords.z = -1.0f;
		clipCoords.w = 1.0f;

		glm::vec4 eyeCoords = glm::inverse(projection) * clipCoords;
		eyeCoords.z = -1.0f;
		eyeCoords.w = 0.0f;

		glm::vec4 rayWorld = glm::inverse(view) * eyeCoords;
		rayWorld = glm::normalize(rayWorld);

		return glm::vec3(rayWorld.x, rayWorld.y, rayWorld.z);
	}

	glm::mat4 GetViewMatrix()
	{
		return calculate_lookAt_matrix(position, position + cameraFront, cameraUp);
	}

	glm::mat4 GetProjectionMatrix()
	{
		if (width == 0 || height == 0)
			return glm::perspective(glm::radians(fov), 1.0f, 0.1f, cameraFar);
		else
			return glm::perspective(glm::radians(fov), width / height, 0.1f, cameraFar);
	}
};

#endif