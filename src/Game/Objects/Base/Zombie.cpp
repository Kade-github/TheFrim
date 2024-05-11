#include "Zombie.h"
#include "../../LightingManager.h"
#include <Game.h>

Zombie::Zombie(glm::vec3 pos) : AI(pos)
{
	m = Model("Assets/Models/zombie_frim.obj");
	m.LoadUVMap("zombie_frim");

	m.scale = glm::vec3(0.7, 0.7, 0.7);


	// raise arms

	Mesh& leftArm = m.GetMesh("LeftArm");
	Mesh& rightArm = m.GetMesh("RightArm");

	leftArm.axis = glm::vec3(0, 0, 1);
	rightArm.axis = glm::vec3(0, 0, 1);

	leftArm.angle = 90;
	rightArm.angle = 90;

	// center them

	leftArm.position += glm::vec3(0.5, 0.5, 0);
	rightArm.position += glm::vec3(0.5, 0.5, 0);

}

void Zombie::Draw()
{
	m.position = position - glm::vec3(0,0.9f,0);
	m.angle = angle;

	// bind shader

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", (float)lightLevel);

	m.Draw();

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", 10.0f);

	Game::instance->shader->Unbind();

	AI::Draw(); // physics and AI pathing
}
