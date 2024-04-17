#ifndef _BLOCKTEST_SCENE_H_
#define _BLOCKTEST_SCENE_H_

#include <Objects/Scene.h>
#include <OpenGL/Texture.h>

class BlockTest : public Scene
{
	unsigned int VAO, VBO, EBO;

	std::vector<GameObject::VVertex> vertices;
	std::vector<unsigned int> indices;

public:
	Texture* t;
	int blockId = 0;

	BlockTest();

	void ChangeBlock();

	void Draw() override;
	void KeyPress(int key) override;
};

#endif