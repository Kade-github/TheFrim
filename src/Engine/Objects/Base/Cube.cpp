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

	glm::vec4 dirt = texture->getSrc(0, 256);
	glm::vec4 grass = texture->getSrc(0, 128);
	glm::vec4 grassSide = texture->getSrc(0, 0);


	// front face
	Game::instance->render->AddQuad(position, glm::vec3(width, height, 0), 0, grassSide, glm::vec3(0.0f, 0.0f, -1.0f), true);

	// back face
	Game::instance->render->AddQuad(glm::vec3(position.x, position.y, position.z + depth), glm::vec3(width, height, 0), 0, grassSide, glm::vec3(0.0f, 0.0f, 1.0f), true);

	// left face
	Game::instance->render->AddQuad(glm::vec3(position.x, position.y, position.z), glm::vec3(0, height, depth), 0, grassSide, glm::vec3(-1.0f, 0.0f, 0.0f), true);

	// right face
	Game::instance->render->AddQuad(glm::vec3(position.x + width, position.y, position.z), glm::vec3(0, height, depth), 0, grassSide, glm::vec3(1.0f, 0.0f, 0.0f), true);

	// top face
	Game::instance->render->AddQuad(glm::vec3(position.x, position.y + height, position.z), glm::vec3(width, 0, 0), depth, grass, glm::vec3(0.0f, 1.0f, 0.0f));

	// bottom face
	Game::instance->render->AddQuad(glm::vec3(position.x, position.y, position.z), glm::vec3(width, 0, 0), depth, dirt, glm::vec3(0.0f, -1.0f, 0.0f));

	Game::instance->render->Draw(texture, Game::instance->shader, camera, rotateAxis, angle);

}
