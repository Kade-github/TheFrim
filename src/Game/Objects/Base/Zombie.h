#ifndef _ZOMBIE_H
#define _ZOMBIE_H

#include "AI.h"
#include <OpenGL/Model.h>
class Zombie : public AI
{
	Model m;

public:
	float tempYaw = 0;

	Zombie(glm::vec3 pos);

	Mesh* torso;
	Mesh* head;
	Mesh* leftArm;
	Mesh* rightArm;
	Mesh* leftLeg;
	Mesh* rightLeg;

	glm::vec3 la;
	glm::vec3 ra;

	glm::vec3 GetFakeDirection()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(tempYaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(tempYaw)) * cos(glm::radians(pitch));
		return glm::normalize(direction);
	}

	void Draw() override;
};

#endif