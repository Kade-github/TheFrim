#include "Entity.h"
#include <Game.h>
#include "../../WorldManager.h"

Entity::Entity(glm::vec3 pos) : GameObject(pos)
{
}

void Entity::CheckCollision(glm::vec3& motion)
{
	// TODO: check collision with blocks
}

void Entity::CheckVerticalCollision(glm::vec3& motion)
{
	// TODO: check vertical collision with blocks
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
