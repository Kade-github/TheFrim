#include "Zombie.h"
#include "../../LightingManager.h"
#include <Game.h>

Zombie::Zombie(glm::vec3 pos) : AI(pos)
{
	m = Model("Assets/Models/zombie_frim.obj");
	m.LoadUVMap("zombie_frim");

	m.scale = glm::vec3(0.7, 0.7, 0.7);


	// raise arms

	leftArm = &m.GetMesh("LeftArm");
	rightArm = &m.GetMesh("RightArm");

	leftArm->axis = glm::vec3(0, 0, 1);
	rightArm->axis = glm::vec3(0, 0, 1);

	leftArm->angle = 90;
	rightArm->angle = 90;

	// center them

	leftArm->position += glm::vec3(0, 0.5, 0);
	rightArm->position += glm::vec3(0, 0.5, 0);

	la = leftArm->position;
	ra = rightArm->position;

	torso = &m.GetMesh("Torso");

	torso->axis = glm::vec3(0, 1, 0);

	head = &m.GetMesh("Head");

	leftLeg = &m.GetMesh("LeftLeg");
	rightLeg = &m.GetMesh("RightLeg");

	speed = 3.5f;

	rotateAxis = glm::vec3(0, 1, 0);

}

void Zombie::Draw()
{
	m.position = position - glm::vec3(0,0.9f,0);
	m.rotateAxis = rotateAxis;

	if (std::abs(yaw - tempYaw) > 0.1f)
	{
		tempYaw = std::lerp(tempYaw, -yaw, Game::instance->deltaTime * 10);

		leftArm->position.x = std::lerp(leftArm->position.x, la.x + front.x * 0.5f, Game::instance->deltaTime * 5);
		leftArm->position.z = std::lerp(leftArm->position.z, la.z + front.z * 0.5f, Game::instance->deltaTime * 5);

		rightArm->position.x = std::lerp(rightArm->position.x, ra.x + front.x * 0.5f, Game::instance->deltaTime * 5);
		rightArm->position.z = std::lerp(rightArm->position.z, ra.z + front.z * 0.5f, Game::instance->deltaTime * 5);


		
	}

	m.angle = tempYaw;

	// bind shader

	Game::instance->shader->Bind();

	//Game::instance->shader->SetUniform1f("lightLevel", (float)lightLevel);


	m.Draw();


	Game::instance->shader->Bind();

	//Game::instance->shader->SetUniform1f("lightLevel", 10.0f);

	Game::instance->shader->Unbind();

	AI::Draw(); // physics and AI pathing
}
