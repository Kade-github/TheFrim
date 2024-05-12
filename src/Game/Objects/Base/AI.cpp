#include "AI.h"
#include "../../WorldManager.h"
#include <Game.h>

glm::vec3 AI::GenerateRandomPosition(float distance)
{
	// get a random position within a distance from the current position

	// get a random angle

	float angle = (rand() % 360) * 3.14159 / 180;

	// get a random distance

	float dist = (rand() % (int)distance) + distance;

	// calculate the new position

	glm::vec3 newPos = glm::vec3((int)(position.x + cos(angle) * dist), (int)position.y, (int)(position.z + sin(angle) * dist));

	Chunk* c = WorldManager::instance->GetChunk(newPos.x, newPos.z);

	if (c == nullptr)
		return glm::vec3();

	// get the lowest block from our position

	for (int y = (int)newPos.y; y > -1; y--)
	{
		if (c->GetBlock(newPos.x, y, newPos.z) > 0)
		{
			newPos.y = y + 2;
			break;
		}
	}


	return newPos;
}

void AI::MoveTo(glm::vec3 pos)
{
	target = glm::vec3((int)pos.x, (int)pos.y, (int)pos.z);

	// get the path to the target

	path.clear();

	glm::vec3 p = position - glm::vec3(-0.5,0.9f,-0.5);

	p = glm::vec3((int)p.x, p.y, (int)p.z);

	float d = glm::distance(target, p);

	while (d > 1)
	{
		// go towards the target (on x and z)

		float xDistance = std::abs(p.x - pos.x);

		bool blocked = false;

		if (xDistance > 1)
		{
			if (p.x < pos.x && CheckBlock(position, glm::vec3(p.x + 1, p.y, p.z)))
			{
				p.x += 1;
			}
			else if (p.x > pos.x && CheckBlock(position, glm::vec3(p.x - 1, p.y, p.z)))
			{
				p.x -= 1;
			}
			else
			{
				// check if we can jump on top of the block

				if (p.x < pos.x && !CheckBlock(position, glm::vec3(p.x + 1, p.y, p.z)) && CheckBlock(position, glm::vec3(p.x + 1, p.y + 1, p.z)))
				{
					path.push_back(glm::vec3(p.x + 1, p.y + 1, p.z));
					p.y += 1;
					p.x += 1;
				}
				else if (p.x > pos.x && !CheckBlock(position, glm::vec3(p.x - 1, p.y, p.z)) && CheckBlock(position, glm::vec3(p.x - 1, p.y + 1, p.z)))
				{
					path.push_back(glm::vec3(p.x - 1, p.y + 1, p.z));
					p.y += 1;
					p.x -= 1;
				}
				else
				{
					blocked = true;
				}
			}
		}

		float zDistance = std::abs(p.z - pos.z);

		if (zDistance > 1)
		{
			if (p.z < pos.z && CheckBlock(position, glm::vec3(p.x, p.y, p.z + 1)))
			{
				p.z += 1;
			}
			else if (p.z > pos.z && CheckBlock(position, glm::vec3(p.x, p.y, p.z - 1)))
			{
				p.z -= 1;
			}
			else
			{
				// check if we can jump on top of the block

				if (p.z < pos.z && !CheckBlock(position, glm::vec3(p.x, p.y, p.z + 1)) && CheckBlock(position, glm::vec3(p.x, p.y + 1, p.z + 1)))
				{
					p.y += 1;
					p.z += 1;
				}
				else if (p.z > pos.z && !CheckBlock(position, glm::vec3(p.x, p.y, p.z - 1)) && CheckBlock(position, glm::vec3(p.x, p.y + 1, p.z - 1)))
				{
					p.y += 1;
					p.z -= 1;
				}
				else
				{
					blocked = true;
				}
				
			}
		}

		if (!DoesBlockExist(glm::vec3(p.x, p.y - 1, p.z)))
		{
			path.push_back(glm::vec3(p.x, p.y - 1, p.z));
			p.y -= 1;
		}
		else
		{
			path.push_back(glm::vec3(p.x, p.y, p.z));
		}

		d = glm::distance(target, p);

		if (blocked || (zDistance <= 1 && xDistance <= 1))
			break;
	}

	target = p;

	for (int i = 0; i < path.size(); i++)
		path[i] += glm::vec3(0, 0.9f, 0);

	target += glm::vec3(0, 0.9f, 0);

	nextPath = true;

}

bool AI::DoesBlockExist(glm::vec3 pos)
{
	Chunk* toChunk = WorldManager::instance->GetChunk(pos.x, pos.z);

	if (toChunk == nullptr)
		return false;

	int toBlock = toChunk->GetBlock(pos.x, pos.y, pos.z);

	return toBlock > 0;
}

bool AI::CheckBlock(glm::vec3 from, glm::vec3 pos)
{
	if (DoesBlockExist(pos))
		return false;

	return true;
}

void AI::MoveToRandom()
{
	glm::vec3 r = GenerateRandomPosition(10);
	if (std::abs(r.y - position.y) > 4)
		return;

	MoveTo(r);
}

void AI::Draw()
{
	// move to target if we have one

	float distance = glm::distance(position, target);

	float sp = speed;

	if (inWater)
		sp = speed / 2;

	if (debug) // draw the path
	{
		Camera* c = Game::instance->GetCamera();

		for (auto p : path)
			c->DrawDebugCube(p, glm::vec3(0.25f, 0.25f, 0.25f));
	}

	if (path.size() != 0)
	{
		glm::vec3 p = path[0];

		// change front to the direction we are going

		float dist = glm::distance(position, p);

		glm::vec3 direction = glm::normalize(p - position);

		yaw = glm::degrees(atan2(direction.z, direction.x));

		glm::vec3 directionToTarget = glm::normalize(target - position);

		aiYaw = glm::degrees(atan2(directionToTarget.z, directionToTarget.x));
		aiPitch = glm::degrees(asin(directionToTarget.y));


		SetDirection();

		forwardVelocity += sp;


		// set our down velocity
		if (isOnGround || inWater)
		{
			if (p.y > position.y)
				downVelocity = jumpStrength;
		}

		// check if we are at the target

		if (dist <= 0.8f)
		{
			nextPath = true;
			path.erase(path.begin());
		}
	}

	if (forwardVelocity > sp)
		forwardVelocity = sp;

	if (forwardVelocity < -sp)
		forwardVelocity = -sp;

	if (strafeVelocity > sp)
		strafeVelocity = sp;

	if (strafeVelocity < -sp)
		strafeVelocity = -sp;


	Entity::Draw(); // physics
}
