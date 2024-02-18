#ifndef _CAMERA_H
#define _CAMERA_H

#pragma once

#include "../../../Include/glm/gtc/type_ptr.hpp"
#include "../../../Include/glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw = 0;
	float pitch = 0;

	float fov = 45.0f;

	void LookAt(glm::vec3 target)
	{
		glm::vec3 direction = glm::normalize(position - target);
		pitch = glm::degrees(asin(direction.y));
		yaw = glm::degrees(atan2(direction.z, direction.x));
		SetDirection();
	}

	void SetDirection()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}

	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(position, position + cameraFront, cameraUp);
	}

	glm::mat4 GetProjectionMatrix(float width, float height)
	{
		return glm::perspective(glm::radians(fov), width / height, 0.1f, 100.0f);
	}
};

#endif