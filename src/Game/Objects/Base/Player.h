#ifndef _PLAYER_H
#define _PLAYER_H

#include <Objects/GameObject.h>
#include "Chunk.h"

class Player : public GameObject
{
public:
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float gravity = 0.03f;
	float jumpStrength = 0.012f;
	float playerSpeed = 0.02f;

	float downVelocity = 0;
	float forwardVelocity = 0;
	float strafeVelocity = 0;

	bool isOnGround = false;

	float yaw = 0;
	float pitch = 0;

	Player(glm::vec3 pos);

	void CheckCollision(glm::vec3& motion);
	void CheckVerticalCollision(glm::vec3& motion);

	glm::vec3 Ray();

	void SetDirection()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
		front = glm::normalize(direction);
	}

	void Draw() override;
};

#endif