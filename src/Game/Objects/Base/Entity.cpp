#include "Entity.h"
#include <Game.h>
#include "../../WorldManager.h"

Entity::Entity(glm::vec3 pos) : GameObject(pos)
{
}

void Entity::CheckCollision(glm::vec3& motion)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	glm::vec3 _to = motion;

	_to.y -= 2;

	glm::vec3 diff = motion - position;

	float sum = abs(diff.x) + abs(diff.y) + abs(diff.z);

	if (sum == 0)
		return;

	if (currentChunk != NULL)
	{
		float x = (motion.x - currentChunk->position.x);
		float z = (motion.z - currentChunk->position.z);

		int _y = currentChunk->doesBlockExist(x, _to.y, z);

		while (_y >= 0)
		{
			x -= diff.x;
			_y = currentChunk->doesBlockExist(x, _to.y, z);
			if (_y < 0)
				break;

			z -= diff.z;
			_y = currentChunk->doesBlockExist(x, _to.y, z);
			if (_y < 0)
				break;

		}
		motion.x = x + currentChunk->position.x;
		motion.z = z + currentChunk->position.z;
	}


}

void Entity::CheckVerticalCollision(glm::vec3& motion)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	isOnGround = false;

	glm::vec3 _to = motion;

	_to.y -= 2;

	if (currentChunk != NULL)
	{
		float x = (motion.x - currentChunk->position.x);
		float z = (motion.z - currentChunk->position.z);

		int _y = currentChunk->doesBlockExist(x, _to.y, z);

		if (_y >= 0)
		{
			if (downVelocity < 0)
			{
				motion.y = _y + 3;
				isOnGround = true;
				downVelocity = 0;
			}
			else
			{
				motion.y = _y - 2;
				downVelocity = 0;
			}
		}
	}
}

glm::vec3 Entity::Ray()
{
	glm::vec3 ray = Game::instance->GetCamera()->cameraFront;

	
	return glm::vec3(0, 0, 0);

}

void Entity::Draw()
{
	SetDirection();

	// gravity

	glm::vec3 _to = position;
	downVelocity -= gravity * Game::instance->deltaTime;

	_to.y += downVelocity;

	CheckVerticalCollision(_to);

	position.y = _to.y;

	glm::vec3 motion = position;

	motion += front * forwardVelocity;

	motion += glm::normalize(glm::cross(front, up)) * strafeVelocity;

	CheckCollision(motion);

	position = motion;

	if (forwardVelocity > (speed * 0.8) * Game::instance->deltaTime)
		forwardVelocity = (speed * 0.8) * Game::instance->deltaTime;

	if (forwardVelocity < -(speed * 0.8) * Game::instance->deltaTime)
		forwardVelocity = -(speed * 0.8) * Game::instance->deltaTime;

	if (strafeVelocity > (speed * 0.8) * Game::instance->deltaTime)
		strafeVelocity = (speed * 0.8) * Game::instance->deltaTime;

	if (strafeVelocity < -(speed * 0.8) * Game::instance->deltaTime)
		strafeVelocity = -(speed * 0.8) * Game::instance->deltaTime;

	forwardVelocity *= 0.8;
	strafeVelocity *= 0.8;

	if (downVelocity < -18 * Game::instance->deltaTime)
		downVelocity = -18 * Game::instance->deltaTime;
}
