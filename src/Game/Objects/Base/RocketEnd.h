#ifndef _ROCKETEND_H
#define _ROCKETEND_H

#include <Objects/GameObject.h>
#include <OpenGL/Model.h>
#include <Objects/3DSprite.h>
#include "../../CreditsReader.h"

class RocketEnd : public GameObject
{
public:
	bool creditsSong = false;
	bool playedLift = false;

	CreditsReader cr;

	Camera2D* creditCam;
	std::vector<Text2D*> credits;

	Sprite3D* fire;

	glm::vec3 smoothPos;
	glm::vec3 fakePos;
	glm::vec3 lightPos;

	Model m;

	RocketEnd(glm::vec3 _pos);

	void Destroy() override;

	void Draw() override;
};

#endif