#ifndef _PLAYER_H
#define _PLAYER_H

#include <Objects/GameObject.h>

class Player : public GameObject
{
public:
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float playerSpeed = 7.0f;

	float yaw = 0;
	float pitch = 0;

	Player(glm::vec3 pos);

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