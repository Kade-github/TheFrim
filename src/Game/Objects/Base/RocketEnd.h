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
	bool switched = false;
	bool takeScreen = false;
	CreditsReader cr;

	Camera2D* creditCam;
	std::vector<Text2D*> credits;

	Sprite2D* black;
	Sprite2D* logo;

	Sprite3D* fire;
	Sprite3D* fire2;
	Sprite3D* fire3;
	Sprite3D* fire4;

	glm::vec3 smoothPos;
	glm::vec3 fakePos;
	glm::vec3 lightPos;

	Model m;

	RocketEnd(glm::vec3 _pos);

	void Destroy() override;

	void Draw() override;
};

#endif