#include "Pig.h"
#include "../../LightingManager.h"
#include <Game.h>
#include "../../Scenes/Gameplay.h"
#include "Hud.h"

Pig::Pig(glm::vec3 pos) : AI(pos)
{
	m = Model("Assets/Models/pig_frim.obj");
	m.LoadUVMap("pig_frim");

	m.scale = glm::vec3(0.7, 0.7, 0.7);

	speed = 0.2f;

	rotateAxis = glm::vec3(0, 1, 0);
	health = 5;

	frontLeftLeg = &m.GetMesh("FrontLeftLeg");
	frontRightLeg = &m.GetMesh("FrontRightLeg");

	backLeftLeg = &m.GetMesh("BackLeftLeg");
	backRightLeg = &m.GetMesh("BackRightLeg");

	frontLeftLeg->axis = glm::vec3(0, 0, 1);
	frontRightLeg->axis = glm::vec3(0, 0, 1);

	backLeftLeg->axis = glm::vec3(0, 0, 1);
	backRightLeg->axis = glm::vec3(0, 0, 1);


	torso = &m.GetMesh("Torso");

	type = AI_Type::TPig;
}

void Pig::Draw()
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	m.position = position - glm::vec3(0, 1.2f, 0);
	m.rotateAxis = rotateAxis;

	if (!Hud::GamePaused)
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

						frontLeftLeg->angle = sin(glfwGetTime() * 7) * 15;
						frontRightLeg->angle = sin(glfwGetTime() * 7) * 15;

						backLeftLeg->angle = -sin(glfwGetTime() * 7) * 15;
						backRightLeg->angle = -sin(glfwGetTime() * 7) * 15;

						if (frontLeftLeg->angle < 0 && !swappedLeft)
						{
							Footstep();
							swappedLeft = true;
						}
						else if (frontLeftLeg->angle > 0 && swappedLeft)
						{
							swappedLeft = false;
						}

						if (backLeftLeg->angle < 0 && !swappedRight)
						{
							Footstep();
							swappedRight = true;
						}
						else if (backLeftLeg->angle > 0 && swappedRight)
						{
							swappedRight = false;
						}
					}
					else
					{
						frontLeftLeg->angle = std::lerp(frontLeftLeg->angle, 0.0f, Game::instance->deltaTime * 5);
						frontRightLeg->angle = std::lerp(frontRightLeg->angle, 0.0f, Game::instance->deltaTime * 5);

						backLeftLeg->angle = std::lerp(backLeftLeg->angle, 0.0f, Game::instance->deltaTime * 5);
						backRightLeg->angle = std::lerp(backRightLeg->angle, 0.0f, Game::instance->deltaTime * 5);
					}
				}
				else
				{
					frontLeftLeg->angle = 0;
					frontRightLeg->angle = 0;

					backLeftLeg->angle = 0;
					backRightLeg->angle = 0;
				}
			}
		}
	}

	if (isOnGround && !init)
	{
		init = true;
		target = position;
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

	bool isAtTarget = glm::distance(target, position) <= 0.9f;
	if (isAtTarget && glfwGetTime() - lastUpdate > 0.1f)
	{
		float random = (rand() % 25) + 25;

		lastUpdate = glfwGetTime() + random;

		MoveToRandom();
	}

	if (redness > 0 && glfwGetTime() > freakOut) // freak out
	{
		freakOut = glfwGetTime() + 2.0f;

		lastUpdate = glfwGetTime() + 2.1f;

		MoveToRandom();
	}
	else if (glfwGetTime() < freakOut)
	{
		float diff = freakOut - glfwGetTime();

		speed = std::lerp(0.2f, 0.6f, diff / 2);

	}
}

