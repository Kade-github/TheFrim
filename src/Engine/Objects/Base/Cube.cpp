#include "Cube.h"

#include "../../Game.h"

Cube::Cube(glm::vec3 pos, float _w, float _h, float _d, Texture* sprSheet) : GameObject::GameObject(pos)
{
	width = _w;
	height = _h;
	depth = _d;

	texture = sprSheet;
}

void Cube::Draw()
{
	// draw 6 faces of the cube

	glm::vec2 src = texture->getSrc(0, 0);

	// front face

	Game::instance->render->AddQuad(position, glm::vec3(width, height, 0), 0, src);

	// back face

	Game::instance->render->AddQuad(glm::vec3(position.x, position.y, position.z + depth), glm::vec3(width, height, 0), 0, src);

	// left face

	Game::instance->render->AddQuad(position, glm::vec3(0, height, depth), 0, src);

	// right face

	Game::instance->render->AddQuad(glm::vec3(position.x + width, position.y, position.z), glm::vec3(0, height, depth), 0, src);

	// top face

	Game::instance->render->AddQuad(glm::vec3(position.x, position.y + height, position.z), glm::vec3(width, 0, 0), depth, src);

	// bottom face

	Game::instance->render->AddQuad(position, glm::vec3(width, 0, 0), depth, src);

	Game::instance->render->Draw(texture, Game::instance->shader, camera);

}
