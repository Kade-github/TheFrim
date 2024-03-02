#include "Entity.h"
#include <Game.h>
#include "../../WorldManager.h"

Entity::Entity(glm::vec3 pos) : GameObject(pos)
{
}

void Entity::CheckCollision(glm::vec3& motion)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	float toY = motion.y - 2;

	glm::vec3 diff = motion - position;

	glm::vec3 rp = position;

	rp.y -= 2;

	float sum = abs(diff.x) + abs(diff.y) + abs(diff.z);

	if (sum == 0)
		return;

	if (currentChunk != NULL)
	{
		// raycast

		float toX = motion.x;
		float toZ = motion.z;

		glm::vec3 ray = position;
		ray.y = toY;

		Block* hit = currentChunk->GetBlock(ray.x, ray.y, ray.z);

		float progress = 0;

		// first raycast (x axis)

		float _lastX = motion.x;

		while (progress < 1)
		{
			ray.x = rp.x + (diff.x * progress);

			toX = (ray.x - currentChunk->position.x);

			hit = currentChunk->GetBlock(ray.x, ray.y, position.z);

			if (hit != NULL)
				break;

			progress += 0.05;
			_lastX = ray.x;
		}

		_lastX -= (diff.x * 0.2f);

		// second raycast (z axis)

		progress = 0;

		ray = position;
		ray.y = toY;

		float _lastZ = motion.z;

		while (progress < 1)
		{
			ray.z = rp.z + (diff.z * progress);

			toZ = (ray.z - currentChunk->position.z);

			hit = currentChunk->GetBlock(position.x, ray.y, ray.z);

			if (hit != NULL)
				break;

			progress += 0.1;
			_lastZ = ray.z;
			
		}

		_lastZ -= (diff.z * 0.2f);

		motion = glm::vec3(_lastX, position.y, _lastZ);
	}
}

void Entity::CheckVerticalCollision(glm::vec3& motion)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	isOnGround = false;

	float toY = motion.y - 2;

	if (downVelocity > 0)
		toY = motion.y;

	glm::vec3 rp = position;
	rp.y -= 2;

	glm::vec3 diff = motion - position;

	float sum = abs(diff.x) + abs(diff.y) + abs(diff.z);

	if (sum == 0)
	{
		return;
	}

	if (currentChunk != NULL)
	{
		// raycast
		glm::vec3 ray = position;

		ray.y = toY;

		int y = motion.y;

		Block* hit = currentChunk->GetBlock(ray.x, ray.y, ray.z);

		float progress = 0;

		while (progress < 1)
		{
			ray.y = rp.y + (diff.y * progress);

			hit = currentChunk->GetBlock(ray.x, ray.y, ray.z);

			if (hit != NULL)
			{
				y = hit->position.y + 3;
				break;
			}

			progress += 0.05;
		}

		if (hit != NULL)
		{
			if (downVelocity <= 0)
			{
				isOnGround = true;
				downVelocity = 0;
				motion.y = y;
			}
			else
			{
				motion.y = y - 4;
				downVelocity = 0;
			}
		}
	}
}

void Entity::Draw()
{
	SetDirection();

	// gravity

	glm::vec3 _to = position;
	downVelocity -= gravity * Game::instance->deltaTime;

	if (downVelocity < -18)
		downVelocity = -18;

	if (downVelocity > 18)
		downVelocity = 18;

	_to.y += downVelocity * Game::instance->deltaTime;

	CheckVerticalCollision(_to);

	position.y = _to.y;

	glm::vec3 motion = position;

	motion += front * (forwardVelocity * Game::instance->deltaTime);

	motion += glm::normalize(glm::cross(front, up)) * (strafeVelocity * Game::instance->deltaTime);

	CheckCollision(motion);

	position = motion;

	forwardVelocity *= 0.8;
	strafeVelocity *= 0.8;
}
