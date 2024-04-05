#ifndef _3DSPRITE_H
#define _3DSPRITE_H

#include "GameObject.h"
#include "2DCamera.h"
#include "2DSprite.h"

class Sprite3D : public GameObject
{
	Camera2D* _camera = nullptr;

	unsigned int VAO, VBO;

	std::vector<GameObject::VVertex> vertices;
	Sprite2D* _spriteRenderer = nullptr;
public:
	float width = 1;
	float height = 1;
	Sprite3D(std::string path, glm::vec3 pos);

	void Draw() override;

	void UpdateSprite();

	void Destroy() override;
};

#endif