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

		float toX = (motion.x - currentChunk->position.x);
		float toZ = (motion.z - currentChunk->position.z);

		float pX = (position.x - currentChunk->position.x);
		float pZ = (position.z - currentChunk->position.z);

		glm::vec3 ray = position;
		ray.y = toY;

		bool hit = currentChunk->doesBlockExist(toX, ray.y, toZ) >= 0;

		float progress = 0;

		// first raycast (x axis)

		float _lastX = motion.x;

		while (progress < 1)
		{
			ray.x = rp.x + (diff.x * progress);

			toX = (ray.x - currentChunk->position.x);

			hit = currentChunk->doesBlockExist(toX, ray.y, pZ) >= 0;

			if (hit)
				break;

			progress += 0.1;
			_lastX = ray.x;
		}

		// second raycast (z axis)

		progress = 0;

		ray = position;
		ray.y = toY;

		float _lastZ = motion.z;

		while (progress < 1)
		{
			ray.z = rp.z + (diff.z * progress);

			toZ = (ray.z - currentChunk->position.z);

			hit = currentChunk->doesBlockExist(pX, ray.y, toZ) >= 0;

			if (hit)
				break;

			progress += 0.1;
			_lastZ = ray.z;
			
		}

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

		float toX = (position.x - currentChunk->position.x);
		float toZ = (position.z - currentChunk->position.z);

		int _lastY = currentChunk->doesBlockExist(toX, ray.y, toZ);

		bool hit = _lastY >= 0;

		float progress = 0;

		while (progress < 1)
		{
			ray.y = rp.y + (diff.y * progress);

			int b = currentChunk->doesBlockExist(toX, ray.y, toZ);

			_lastY = b;
			hit = b >= 0;

			if (hit)
				break;

			progress += 0.1;
		}

		if (_lastY >= 0)
		{
			if (downVelocity <= 0)
			{
				isOnGround = true;
				downVelocity = 0;
				motion.y = _lastY + 3;
			}
			else
			{
				if (_lastY > position.y)
				{
					motion.y = _lastY - 1;
					downVelocity = 0;
				}
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
