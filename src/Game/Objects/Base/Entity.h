#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <Objects/GameObject.h>

class Entity : public GameObject
{
public:
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	float gravity = 19.0f;
	float jumpStrength = 7.8f;
	float speed = 9.0f;

	float downVelocity = 0;
	float forwardVelocity = 0;
	float strafeVelocity = 0;

	bool isOnGround = false;

	float yaw = 0;
	float pitch = 0;

	Entity(glm::vec3 pos);

	void CheckCollision(glm::vec3& motion, float down);
	void CheckVerticalCollision(glm::vec3& motion);

	bool RayTo(glm::vec3& to, bool inside = false);

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