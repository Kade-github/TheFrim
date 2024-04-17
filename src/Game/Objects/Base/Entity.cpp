#include "Entity.h"
#include <Game.h>
#include "../../Scenes/Gameplay.h"
#include "../../WorldManager.h"
#include "../../MusicManager.h"

Entity::Entity(glm::vec3 pos) : GameObject(pos)
{
	Texture* t = Texture::createWithImage("Assets/Textures/entity_shadow.png");

	shadow = new Sprite3D(t, glm::vec3(0,0,0));
	shadow->t->spriteSheet.Load("Assets/Textures/entity_shadow.xml", shadow->t->width, shadow->t->height);
	shadow->rotateAxis = glm::vec3(1, 0, 0);
}

void Entity::Footstep()
{
	Chunk* c = WorldManager::instance->GetChunk(position.x, position.z);

	float pitch = 1.0f + ((rand() % 20) - 10) / 100.0f;

	// get block under player

	int y = (int)position.y - 2;

	if (c->DoesBlockExist(position.x, y, position.z))
	{
		subChunk* sb = c->GetSubChunk(y);

		if (sb == nullptr)
			return;

		glm::vec3 _world = c->WorldToChunk(position);

		Block* b = sb->getBlock(_world.x, _world.z);

		if (b != nullptr)
		{
			switch (b->soundType)
			{
			case SoundType::S_GRASS:
				MusicManager::GetInstance()->PlaySFX("grass_sfx", pitch, "walk");
				break;
			case SoundType::S_STONE:
				MusicManager::GetInstance()->PlaySFX("stone_sfx", pitch, "walk");
				break;
			case SoundType::S_WOOD:
				MusicManager::GetInstance()->PlaySFX("wood_sfx", pitch, "walk");
				break;
			}
		}
	}
}

void Entity::Launch(glm::vec3 direction, float force)
{
	downVelocity += force;

	forwardVelocity += direction.x * force;
	strafeVelocity += direction.z * force;
}

void Entity::Launch(glm::vec3 direction, float force, float upForce)
{
	forwardVelocity += direction.x * force;
	strafeVelocity += direction.z * force;

	downVelocity += upForce;
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

			if (currentChunk != nullptr)
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

			if (currentChunk != nullptr)
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

	bool wasOnGround = isOnGround;

	isOnGround = false;

	float toY = motion.y - 1.8;

	if (!isCreature)
		toY = motion.y;

	glm::vec3 rp = position;
	rp.y -= 1.8;

	if (!isCreature)
		rp.y = position.y;

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
				if (!wasOnGround && isCreature)
					Footstep();

				isOnGround = true;
				downVelocity = 0;
				motion.y = _lastY + 2.8;
				if (!isCreature)
					motion.y = _lastY + 1;
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
	if (!Hud::GamePaused)
	{

		SetDirection();

		// gravity

		downVelocity -= gravity * Game::instance->deltaTime;

		if (downVelocity < -18)
			downVelocity = -18;

		if (downVelocity > 18)
			downVelocity = 18;

		glm::vec3 preMotion = position;

		glm::vec3 motion = position;

		motion.y += downVelocity * Game::instance->deltaTime;

		CheckVerticalCollision(motion);

		motion += front * (forwardVelocity * Game::instance->deltaTime);

		motion += glm::normalize(glm::cross(front, up)) * (strafeVelocity * Game::instance->deltaTime);

		if (isCreature)
		{
			CheckCollision(motion, 0.4);

			CheckCollision(motion, 0.8);

			CheckCollision(motion, 1.8);
		}
		else
		{
			CheckCollision(motion, 0);
		}

		// check if motion is NaN

		if (motion.x != motion.x || motion.y != motion.y || motion.z != motion.z)
			motion = preMotion;

		position = motion;

		if (isCreature)
		{
			if (forwardVelocity != 0)
				forwardVelocity *= 0.8;
			if (strafeVelocity != 0)
				strafeVelocity *= 0.8;
		}
		else
		{
			if (forwardVelocity != 0)
				forwardVelocity *= 0.95;
			if (strafeVelocity != 0)
				strafeVelocity *= 0.95;
		}

		if (forwardVelocity <= 0.01 && forwardVelocity >= -0.01)
			forwardVelocity = 0;

		if (strafeVelocity <= 0.01 && strafeVelocity >= -0.01)
			strafeVelocity = 0;
	}

	if (isCreature)
		shadow->position = position - glm::vec3(0, 1.7, 0.5);
	else
		shadow->position = position - glm::vec3(0, 0.5, 0.5);

	Chunk* currentChunk = WorldManager::instance->GetChunk(position.x, position.z);

	bool drawShadow = true;

	if (currentChunk != nullptr)
	{
		int topBlock = currentChunk->GetHighestBlock(position.x, position.z);

		float diff = topBlock - position.y;

		shadow->position.y = topBlock + 1.05;

		if (diff > -6)
		{
			if (diff < -4 && diff > -6)
				shadow->src = shadow->t->spriteSheet.GetUVFlip("entity_shadow_25");
			else if (diff < -2 && diff > -4)
				shadow->src = shadow->t->spriteSheet.GetUVFlip("entity_shadow_50");
			else if (diff <= 0 && diff > -2)
				shadow->src = shadow->t->spriteSheet.GetUVFlip("entity_shadow_100");

			shadow->UpdateSprite();
		}
		else
			drawShadow = false;
	}

	shadow->angle = 90;

	if (drawShadow)
		shadow->Draw();
}
