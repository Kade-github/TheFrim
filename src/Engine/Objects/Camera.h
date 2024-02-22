#ifndef _CAMERA_H
#define _CAMERA_H

#pragma once

#include "../../../Include/glm/gtc/type_ptr.hpp"
#include "../../../Include/glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	float width = 1280;
	float height = 720;

	float cameraFar = 100.0f;

	glm::vec3 lightPos = glm::vec3(0.0f, 5.0f, 0.0f);
	glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);

	glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float yaw = 0;
	float pitch = 0;

	float fov = 75.0f;

	void LookAt(glm::vec3 target)
	{
		glm::vec3 direction = glm::normalize(position - target);
		pitch = glm::degrees(asin(direction.y));
		yaw = glm::degrees(atan2(direction.z, direction.x));
		SetDirection();
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

	void SetDirection()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		cameraFront = glm::normalize(direction);
	}

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
		return glm::lookAt(position, position + cameraFront, cameraUp);
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