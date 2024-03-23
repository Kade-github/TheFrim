#include "Entity.h"
#include <Game.h>
#include "../../WorldManager.h"
#include "../../MusicManager.h"

Entity::Entity(glm::vec3 pos) : GameObject(pos)
{
}

void Entity::Footstep()
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(position.x, position.z);

	if (currentChunk != NULL)
	{
		if (currentChunk->DoesBlockExist(position.x, position.y - 1, position.z))
		{
			if (!MusicManager::GetInstance()->ChannelIsPlaying("sfx"))
			{
				MusicManager::GetInstance()->PlaySFX("grass_sfx");
			}
		}
	}
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
			glm::vec3 lastRay = ray;
			_lastX = lastRay.x;
			ray.x = rp.x + (diff.x * progress);

			toX = ray.x;

			currentChunk = WorldManager::instance->GetChunk(ray.x, ray.z);

			hit = currentChunk->DoesBlockExist(toX, ray.y, pZ);

			if (hit)
				break;

			progress += 0.1;
		}

		// second raycast (z axis)

		progress = 0;

		ray = position;
		ray.y = toY; 

		float _lastZ = motion.z;

		while (progress < 1)
		{
			glm::vec3 lastRay = ray;
			_lastZ = lastRay.z;
			ray.z = rp.z + (diff.z * progress);

			toZ = ray.z;

			currentChunk = WorldManager::instance->GetChunk(ray.x, ray.z);

			hit = currentChunk->DoesBlockExist(_lastX, ray.y, toZ);

			if (hit)
				break;

			progress += 0.1;
		}

		motion = glm::vec3(_lastX, motion.y, _lastZ);
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
					motion.y = _lastY - 0.05f;
					downVelocity = 0;
				}
			}
		}
	}
}

bool Entity::RayTo(glm::vec3& to, bool inside)
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
		glm::vec3 lastRay = ray;
		ray = start + (diff * progress);
		if (inside)
			lastRay = ray;
		Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

		if (c != nullptr)
			if (c->DoesBlockExist(ray.x, ray.y, ray.z))
			{
				to = lastRay;
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

	CheckCollision(motion, 0.4);

	CheckCollision(motion, 0.8);

	CheckCollision(motion, 1.8);

	position = motion;

	// footstep

	if (isOnGround && (forwardVelocity != 0 || strafeVelocity != 0))
		Footstep();

	if (forwardVelocity != 0)
		forwardVelocity *= 0.8;
	if (strafeVelocity != 0)
		strafeVelocity *= 0.8;

	if (forwardVelocity <= 0.01 && forwardVelocity >= -0.01)
		forwardVelocity = 0;

	if (strafeVelocity <= 0.01 && strafeVelocity >= -0.01)
		strafeVelocity = 0;
}
