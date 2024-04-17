#ifndef _3DTEXT_H
#define _3DTEXT_H

#include "GameObject.h"
#include "2DText.h"
#include "2DCamera.h"

class Text3D : public GameObject
{
	Camera2D* _camera = nullptr;

	unsigned int VAO, VBO;

	std::vector<GameObject::VVertex> vertices;
	Text2D* _textRenderer = nullptr;
public:
	float angleZ = 0;
	float angleY = 0;
	Text3D(std::string text, std::string font, glm::vec3 pos, glm::vec4 color, int size = 12);

	void UpdateText(std::string text);

	void Draw() override;

	void Destroy() override;
};

#endif