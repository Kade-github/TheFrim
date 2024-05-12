#include "Zombie.h"
#include "../../LightingManager.h"
#include <Game.h>
#include "../../Scenes/Gameplay.h"
#include "Hud.h"

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

	leftLeg->axis = glm::vec3(0, 0, 1);
	rightLeg->axis = glm::vec3(0, 0, 1);

	speed = 3.5f;

	rotateAxis = glm::vec3(0, 1, 0);

}

void Zombie::Draw()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	m.position = position - glm::vec3(0,0.9f,0);
	m.rotateAxis = rotateAxis;

	if (isOnGround && !init)
	{
		init = true;
		target = position;
	}

	if (!Hud::GamePaused)
	{
		tempYaw = std::lerp(tempYaw, -yaw, Game::instance->deltaTime * 10);

		leftArm->position.x = std::lerp(leftArm->position.x, la.x + front.x * 0.5f, Game::instance->deltaTime * 5);
		leftArm->position.z = std::lerp(leftArm->position.z, la.z + front.z * 0.5f, Game::instance->deltaTime * 5);

		rightArm->position.x = std::lerp(rightArm->position.x, ra.x + front.x * 0.5f, Game::instance->deltaTime * 5);
		rightArm->position.z = std::lerp(rightArm->position.z, ra.z + front.z * 0.5f, Game::instance->deltaTime * 5);

		if (path.size() != 0)
		{
			// leg movement

			float dist = glm::distance(position, path[0]);

			if (dist > 0.5f)
			{
				if (isOnGround)
				{
					leftLeg->angle = -sin(glfwGetTime() * 5) * 15;
					rightLeg->angle = sin(glfwGetTime() * 5) * 15;

					if (leftLeg->angle < 0 && !swappedLeft)
					{
						Footstep();
						swappedLeft = true;
					}
					else if (leftLeg->angle > 0 && swappedLeft)
					{
						swappedLeft = false;
					}

					if (rightLeg->angle < 0 && !swappedRight)
					{
						Footstep();
						swappedRight = true;
					}
					else if (rightLeg->angle > 0 && swappedRight)
					{
						swappedRight = false;
					}
				}
				else
				{
					leftLeg->angle = std::lerp(leftLeg->angle, 0.0f, Game::instance->deltaTime * 5);
					rightLeg->angle = std::lerp(rightLeg->angle, 0.0f, Game::instance->deltaTime * 5);
				}
			}
			else
			{
				leftLeg->angle = 0;
				rightLeg->angle = 0;
			}
		}
		else
		{
			leftLeg->angle = std::lerp(leftLeg->angle, 0.0f, Game::instance->deltaTime * 5);
			rightLeg->angle = std::lerp(rightLeg->angle, 0.0f, Game::instance->deltaTime * 5);
		}
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

	float dist = glm::distance(position, gp->player->position);

	if (dist <= 1.5)
		Attack();

	if (dist <= 10 && !gp->player->noTarget)
	{
		if (gp->ticks % 10 != 0)
			return;

		// check if we can see the player

		glm::vec3 direction = gp->player->position - position;

		float angle = glm::degrees(glm::acos(glm::dot(glm::normalize(direction), glm::normalize(front))));

		if (angle > 70)
		{
			canSeePlayer = false;
			return;
		}

		// ray cast

		glm::vec3 p = gp->player->position;

		bool collision = RayTo(p);

		if (!collision)
		{
			if (glfwGetTime() - lastUpdate > 0.1f)
			{
				lastUpdate = glfwGetTime();
				MoveTo(gp->player->position);
			}
			canSeePlayer = true;
		}
		else
		{
			canSeePlayer = false;
		}
	}
	else
	{
		bool isAtTarget = glm::distance(target, position) <= 0.9f;
		if (isAtTarget && glfwGetTime() - lastUpdate > 0.1f)
		{
			float random = rand() % 25;

			lastUpdate = glfwGetTime() + random;

			MoveToRandom();
		}
	}
}

void Zombie::Attack()
{
	if (glfwGetTime() > attackCooldown)
	{
		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		gp->player->Hurt(2.5f);

		attackCooldown = glfwGetTime() + 1.0f;
	}
}
