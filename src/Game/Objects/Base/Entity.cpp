#include "Entity.h"
#include <Game.h>
#include "../../WorldManager.h"

Entity::Entity(glm::vec3 pos) : GameObject(pos)
{
}

void Entity::CheckCollision(glm::vec3& motion, float down)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	float toY = motion.y - down;

	glm::vec3 diff = motion - position;

	glm::vec3 rp = position;

	rp.y -= down;

	float sum = abs(diff.x) + abs(diff.y) + abs(diff.z);

	if (sum == 0)
		return;

	if (currentChunk != NULL)
	{
		// raycast

		float toX = motion.x;
		float toZ = motion.z;


		float pX = position.x;
		float pZ = position.z;

		glm::vec3 ray = position;
		ray.y = toY;

		glm::vec3 _world = currentChunk->WorldToChunk(ray);

		if (_world.x == 16)
			toX--;

		if (_world.z == 16)
			toZ--;

		bool hit = false;

		float progress = 0;

		// first raycast (x axis)

		float _lastX = motion.x;

		while (progress < 1)
		{
			ray.x = rp.x + (diff.x * progress);

			toX = ray.x;

			hit = currentChunk->DoesBlockExist(toX, ray.y, pZ);

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

			toZ = ray.z;

			hit = currentChunk->DoesBlockExist(pX, ray.y, toZ);

			if (hit)
				break;

			progress += 0.1;
			_lastZ = ray.z;

		}

		motion = glm::vec3(_lastX - (diff.x * 0.1f), motion.y, _lastZ - (diff.z * 0.1f));
	}
}

void Entity::CheckVerticalCollision(glm::vec3& motion)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	isOnGround = false;

	float toY = motion.y - 1.8;

	glm::vec3 rp = position;
	rp.y -= 1.8;

	if (downVelocity > 0)
	{
		toY = motion.y;
		rp.y = position.y;
	}


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

		float toX = position.x;
		float toZ = position.z;

		glm::vec3 _world = currentChunk->WorldToChunk(ray);

		if (_world.x == 16)
			toX--;

		if (_world.z == 16)
			toZ--;

		int _lastY = -1;

		bool hit = false;

		float progress = 0;

		while (progress < 1)
		{
			ray.y = rp.y + (diff.y * progress);


			hit = currentChunk->DoesBlockExist(toX, ray.y, toZ);

			if (hit)
			{
				_lastY = (int)(ray.y);
				break;
			}

			progress += 0.1;
		}

		if (_lastY >= 0)
		{
			if (downVelocity <= 0)
			{
				isOnGround = true;
				downVelocity = 0;
				motion.y = _lastY + 2.8;
			}
			else
			{
				if (_lastY > position.y)
				{
					motion.y = _lastY - 1.2;
					downVelocity = 0;
				}
			}
		}
	}
}

bool Entity::RayTo(glm::vec3& to)
{
	glm::vec3 diff = to - position;

	float sum = diff.x + diff.y + diff.z;

	if (sum == 0)
		return false;

	float progress = 0;

	glm::vec3 ray = position;

	glm::vec3 start = position;

	while (progress < 1)
	{
		ray = start + (diff * progress);

		Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

		if (c != nullptr)
			if (c->DoesBlockExist(ray.x, ray.y, ray.z))
			{
				to = ray;
				return true;
			}

		progress += 0.05;
	}

	return false;
}

void Entity::Draw()
{
	SetDirection();

	// gravity

	downVelocity -= gravity * Game::instance->deltaTime;

	if (downVelocity < -18)
		downVelocity = -18;

	if (downVelocity > 18)
		downVelocity = 18;

	glm::vec3 motion = position;

	motion.y += downVelocity * Game::instance->deltaTime;

	CheckVerticalCollision(motion);

	motion += front * (forwardVelocity * Game::instance->deltaTime);

	motion += glm::normalize(glm::cross(front, up)) * (strafeVelocity * Game::instance->deltaTime);

	CheckCollision(motion, 0);

	CheckCollision(motion, 1.8);

	position = motion;

	if (forwardVelocity != 0)
		forwardVelocity *= 0.8;
	if (strafeVelocity != 0)
		strafeVelocity *= 0.8;

	if (forwardVelocity <= 0.01 && forwardVelocity >= -0.01)
		forwardVelocity = 0;

	if (strafeVelocity <= 0.01 && strafeVelocity >= -0.01)
		strafeVelocity = 0;
}
