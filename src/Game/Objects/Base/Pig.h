#ifndef _PIG_H
#define _PIG_H

#include "AI.h"
#include <OpenGL/Model.h>
class Pig : public AI
{
	Model m;
	bool init = false;
public:
	float tempYaw = 0;
	float attackCooldown = 0;
	float lastUpdate = 0;
	Pig(glm::vec3 pos);

	float freakOut = 0;

	bool swappedLeft = false;
	bool swappedRight = false;

	Mesh* torso;
	Mesh* head;
	Mesh* frontLeftLeg;
	Mesh* frontRightLeg;
	Mesh* backLeftLeg;
	Mesh* backRightLeg;

	glm::vec3 GetFakeDirection()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(tempYaw)) * cos(glm::radians(pitch));
		direction.y = sin(glm::radians(pitch));
		direction.z = sin(glm::radians(tempYaw)) * cos(glm::radians(pitch));
		return glm::normalize(direction);
	}

	void Noise() override;

	void Draw() override;
};

#endif