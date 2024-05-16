#ifndef _ENTITY_H_
#define _ENTITY_H_

#include <Objects/GameObject.h>
#include <Objects/3DSprite.h>
#include "Block.h"

class Entity : public GameObject
{
	int lightUpdate = 0;
public:
	Sprite3D* shadow;
	
	glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

	int lightLevel = 10;

	bool isCreature = true;

	bool topWater = false;
	bool inWater = false;

	float gravity = 20.0f;
	float jumpStrength = 7.8f;
	float speed = 9.0f;

	bool tiny = false;

	float downVelocity = 0;
	float forwardVelocity = 0;
	float strafeVelocity = 0;

	bool isOnGround = false;

	float yaw = 0;
	float pitch = 0;

	Entity(glm::vec3 pos);
    ~Entity();

	void Footstep();
	void FootstepSound(Block* b, std::string appendName = "", float pitchAdd = 0);

	void Launch(glm::vec3 direction, float force);
	void Launch(glm::vec3 direction, float force, float upForce);

	void CheckCollision(glm::vec3& motion, float down);
	void CheckVerticalCollision(glm::vec3& motion);

	bool RayTo(glm::vec3& to, bool inside = false);
	bool RayToIncludeWater(glm::vec3& to, bool inside = false);

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