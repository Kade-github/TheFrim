#include "Cube.h"

Cube::Cube(glm::vec3 pos, float _w, float _h, float _d, Model* m, Texture* sprSheet) : GameObject::GameObject(pos)
{
	width = _w;
	height = _h;
	depth = _d;

	texture = sprSheet;
	model = m;
}

void Cube::Draw()
{
	Game::instance->render->AddDrawCall(position, model, texture, shader, rotateAxis, angle);
}
