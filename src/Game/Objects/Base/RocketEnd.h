#ifndef _ROCKETEND_H
#define _ROCKETEND_H

#include <Objects/GameObject.h>
#include <OpenGL/Model.h>

class RocketEnd : public GameObject
{
public:
	RocketEnd(glm::vec3 _pos);
	~RocketEnd();

	void Update(float deltaTime);
	void Render();
};

#endif