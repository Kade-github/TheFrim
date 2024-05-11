#ifndef _ZOMBIE_H
#define _ZOMBIE_H

#include "AI.h"
#include <OpenGL/Model.h>
class Zombie : public AI
{
	Model m;

public:

	Zombie(glm::vec3 pos);

	Mesh* torso;
	Mesh* head;
	Mesh* leftLeg;
	Mesh* rightLeg;

	void Draw() override;
};

#endif