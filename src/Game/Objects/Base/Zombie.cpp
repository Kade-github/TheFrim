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

	leftArm = &m.GetMesh("LeftArm");
	rightArm = &m.GetMesh("RightArm");

	torso = &m.GetMesh("Torso");

	torso->axis = glm::vec3(0, 1, 0);

	head = &m.GetMesh("Head");

	leftLeg = &m.GetMesh("LeftLeg");
	rightLeg = &m.GetMesh("RightLeg");

	leftLeg->axis = glm::vec3(0, 0, 1);
	rightLeg->axis = glm::vec3(0, 0, 1);

	speed = 0.2f;

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


	if (!Hud::GamePaused) // animation stuff
	{

		if (health <= 0)
		{
			// go away
			dying = true;

			m.scale = glm::vec3(std::lerp(m.scale.x, 0.0f, Game::instance->deltaTime * 2));

			if (m.scale.x <= 0.1f)
				dead = true;
		}

		if (!dying && !dead)
		{
			tempYaw = std::lerp(tempYaw, -yaw, Game::instance->deltaTime * 5);

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
	}

	m.angle = tempYaw;

	// bind shader

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", (float)lightLevel);

	Game::instance->shader->SetUniform1f("redness", redness);

	m.Draw();

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", 10.0f);
	Game::instance->shader->SetUniform1f("redness", 0.0f);

	Game::instance->shader->Unbind();

	AI::Draw(); // physics and AI pathing

	float dist = glm::distance(position, gp->player->position);

	if (dist <= 2.0 && !dead && !dying && !gp->player->noTarget)
		Attack();

	if (dist <= 10 && !gp->player->noTarget)
	{
		// check if we can see the player

		glm::vec3 direction = gp->player->position - position;

		float angle = glm::degrees(glm::acos(glm::dot(glm::normalize(direction), glm::normalize(front))));

		if (angle > 70 && dist > 4.0f)
		{
			canSeePlayer = false;
			return;
		}

		// ray cast

		glm::vec3 p = gp->player->position;

		bool wasSeeing = canSeePlayer;

		canSeePlayer = !RayTo(p);

		if (!wasSeeing)
			lastUpdate = glfwGetTime() - 0.1f;
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

	if (canSeePlayer)
	{
		if (glfwGetTime() - lastUpdate > 0.25f)
		{
			lastUpdate = glfwGetTime();
			MoveTo(gp->player->position);
		}
	}
}

void Zombie::Attack()
{
	if (glfwGetTime() > attackCooldown)
	{
		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		glm::vec3 dir = glm::normalize(gp->player->position - (position + front));

		gp->player->Hurt(2.5f, dir);

		attackCooldown = glfwGetTime() + 1.0f;

		MoveTo(gp->player->position);
	}
}
