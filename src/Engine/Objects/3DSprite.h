#ifndef _3DSPRITE_H
#define _3DSPRITE_H

#include "GameObject.h"
#include "2DCamera.h"
#include "2DSprite.h"

class Sprite3D : public GameObject
{
	unsigned int VAO, VBO;

	std::vector<GameObject::VVertex> vertices;
public:

	bool depth = true;

	bool flipHorizontal = false;

	float angleZ = 0;
	float angleY = 0;
	glm::vec4 src = glm::vec4(0, 0, 1, 1);
	Texture* t;
	float width = 1;
	float height = 1;
	Sprite3D(std::string path, glm::vec3 pos);
	Sprite3D(Texture* _t, glm::vec3 pos);

	void Draw() override;

	void LookAt(glm::vec3 target);

	void UpdateSprite();

	void Destroy() override;
};

#endif