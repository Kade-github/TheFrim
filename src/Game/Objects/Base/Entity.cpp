#include "Entity.h"
#include <Game.h>
#include "../../Scenes/Gameplay.h"
#include "../../WorldManager.h"
#include "../../MusicManager.h"
#include "../../LightingManager.h"
#include "../../MobManager.h"

Entity::Entity(glm::vec3 pos) : GameObject(pos)
{
	lightUpdate = glfwGetTime();
	Texture* t = Texture::createWithImage("Assets/Textures/entity_shadow.png");

	shadow = new Sprite3D(t, glm::vec3(0,0,0));
	shadow->t->spriteSheet.Load("Assets/Textures/entity_shadow.xml", shadow->t->width, shadow->t->height);
	shadow->rotateAxis = glm::vec3(1, 0, 0);
}

void Entity::Footstep()
{
	Chunk* c = WorldManager::instance->GetChunk(position.x, position.z);

	if (c == nullptr)
		return;

	// get block under player

	int y = (int)position.y - 2;

	glm::vec3 block = glm::vec3(position.x, y, position.z);

	if (c->DoesBlockExist(position.x, y, position.z) && lastFootstep != block)
	{
		lastFootstep = block;
		subChunk& sb = c->GetSubChunk(y);

		if (sb.y == -1)
			return;

		glm::vec3 _world = c->WorldToChunk(position);

		if (_world.x == 16)
			_world.x = 15;
		if (_world.z == 16)
			_world.z = 15;

		Block* b = sb.getBlock(_world.x, _world.z);

		FootstepSound(b);
	}
}

void Entity::FootstepSound(Block* b, std::string append, float pitchAdd)
{
	if (!playSounds)
		return;

	float pitch = 1.0f + ((rand() % 20) - 10) / 100.0f;

	if (pitchAdd != 0)
		pitch = 1.0f + (pitchAdd / 100.0f);

	if (b != nullptr)
	{
		switch (b->soundType)
		{
		case SoundType::S_SAND:
		case SoundType::S_GRASS:
			MusicManager::GetInstance()->PlaySFX("grass_sfx", position, pitch, "walk" + append);
			break;
		case SoundType::S_STONE:
			MusicManager::GetInstance()->PlaySFX("stone_sfx", position, pitch, "walk" + append);
			break;
		case SoundType::S_WOOD:
			MusicManager::GetInstance()->PlaySFX("wood_sfx", position, pitch, "walk" + append);
			break;
		}
	}
}

void Entity::Launch(glm::vec3 direction, float force)
{
	forwardVelocity += direction.x * force;
	strafeVelocity += direction.z * force;
}

void Entity::Launch(glm::vec3 direction, float force, float upForce)
{
	forwardVelocity += direction.x * force;
	strafeVelocity += direction.z * force;

	downVelocity = upForce;

}

void Entity::CheckCollision(glm::vec3& motion, float down)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	float toY = motion.y - down;

	glm::vec3 diff = motion - (position - glm::vec3(0, 0.2, 0));

	glm::vec3 rp = position - glm::vec3(0, 0.2, 0);

	rp.y -= down;

	float sum = abs(diff.x) + abs(diff.y) + abs(diff.z);

	if (sum == 0)
		return;

	if (currentChunk != NULL)
	{
		// raycast

		float toX = motion.x;
		float toZ = motion.z;

		float initialX = motion.x;
		float initialZ = motion.z;

		float pX = position.x;
		float pZ = position.z;

		glm::vec3 ray = position;
		ray.y = toY;

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
			{
				int type = currentChunk->GetBlock(ray.x, ray.y, ray.z);
				hit = type > 0 && type != WATER && type != TORCH;
			}

			if (hit)
			{
				progress -= 0.5f;
				_lastX = rp.x + (diff.x * progress);
				break;
			}

			progress += 0.05;
		}

		// second raycast (z axis)

		progress = 0;

		ray = position - glm::vec3(0, 0.2, 0);
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
			{
				int type = currentChunk->GetBlock(ray.x, ray.y, ray.z);
				hit = type > 0 && type != WATER && type != TORCH;
			}

			if (hit)
			{
				progress -= 0.5f;
				_lastZ = rp.z + (diff.z * progress);
				break;
			}

			progress += 0.05;
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
		ray.x = position.x;
		ray.z = position.z;

		float toX = (int)position.x + 0.5;
		float toZ = (int)position.z + 0.5;

		int _lastY = -1;

		bool hit = false;

		float progress = 0;

		Camera* cam = Game::instance->GetCamera();

		while (progress < 1)
		{
			ray.y = rp.y + (diff.y * progress);

			int type = currentChunk->GetBlock(ray.x, ray.y, ray.z);
			hit = type > 0 && type != WATER && type != TORCH;

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
			if (_lastY > position.y)
			{
				motion.y = _lastY - 0.05f;
				downVelocity = 0;
			}
		}
	}
}

bool Entity::RayToCustom(glm::vec3 s, glm::vec3& to, bool inside)
{
	glm::vec3 diff = to - position;

	float sum = diff.x + diff.y + diff.z;

	if (sum == 0)
		return false;

	float progress = 0;

	glm::vec3 ray = s;

	glm::vec3 start = s;

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	Camera* cam = Game::instance->GetCamera();

	std::vector<AI*> mobs = gp->mm->mobs;

	while (progress < 1)
	{
		glm::vec3 lastRay = ray;
		ray = start + (diff * progress);
		if (inside)
		{
			lastRay = ray;

			for (int i = 0; i < mobs.size(); i++)
			{
				if (mobs[i]->IsPositionInMe(ray))
				{
					to = lastRay;
					return false;
				}
			}
		}
		Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

		if (c != nullptr)
		{
			int type = c->GetBlock(ray.x, ray.y, ray.z);
			if (type > 0 && type != WATER)
			{
				to = lastRay;
				return true;
			}
		}

		progress += 0.01;

		//cam->DrawDebugCube(ray, glm::vec3(0.02f));
	}

	return false;
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

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;

	Camera* cam = Game::instance->GetCamera();

	std::vector<AI*> mobs = gp->mm->mobs;

	while (progress < 1)
	{
		glm::vec3 lastRay = ray;
		ray = start + (diff * progress);
		if (inside)
		{
			lastRay = ray;

			for (int i = 0; i < mobs.size(); i++)
			{
				if (mobs[i]->IsPositionInMe(ray))
				{
					to = lastRay;
					return false;
				}
			}
		}
		Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

		if (c != nullptr)
		{
			int type = c->GetBlock(ray.x, ray.y, ray.z);
			if (type > 0 && type != WATER)
			{
				to = lastRay;
				return true;
			}
		}

		progress += 0.05;

		//cam->DrawDebugCube(ray, glm::vec3(0.05f));
	}

	return false;
}

bool Entity::RayToIncludeWater(glm::vec3& to, bool inside)
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
		{
			int type = c->GetBlock(ray.x, ray.y, ray.z);
			if (type == WATER)
			{
				to = lastRay;
				return true;
			}
		}

		progress += 0.05;
	}

	return false;
}

void Entity::Draw()
{
	// water check

	glm::vec3 ray = position - glm::vec3(0, 0.8, 0);
	glm::vec3 ray2 = position - glm::vec3(0, 1.8, 0);

	topWater = RayToIncludeWater(ray, true);
	if (isCreature)
		inWater = topWater || RayToIncludeWater(ray2, true);
	else
		inWater = topWater;

	if (playSounds)
	{
		if (inWater && !waterSoundEntrance)
		{
			waterSoundEntrance = true;

			float pitch = 1.0f + ((rand() % 20) - 10) / 100.0f;

			MusicManager::GetInstance()->PlaySFX("water_in", position, pitch, "enter");
		}
		else if (!inWater && waterSoundEntrance)
		{
			waterSoundEntrance = false;

			float pitch = 1.0f + ((rand() % 20) - 10) / 100.0f;

			MusicManager::GetInstance()->PlaySFX("water_out", position, pitch, "exit");
		}

	}
	if (glfwGetTime() - lightUpdate > 0.1)
	{
		lightUpdate = glfwGetTime();

		lightLevel = LightingManager::GetInstance()->GetLightLevel(position);
	}

	if (!Hud::GamePaused)
	{

		SetDirection();

		// gravity

		float boyancy = 10.0f;

		if (inWater)
		{
			downVelocity -= (boyancy * 0.25f) * Game::instance->deltaTime;

			if (downVelocity > boyancy)
				downVelocity = boyancy;
			if (downVelocity < -boyancy * 0.25f)
				downVelocity = -boyancy * 0.25f;
		}
		else
			downVelocity -= gravity * Game::instance->deltaTime;

		if (downVelocity < -18)
			downVelocity = -18;

		if (downVelocity > 18)
			downVelocity = 18;

		glm::vec3 preMotion = position;

		glm::vec3 motion = position;

		motion.y += downVelocity * Game::instance->deltaTime;

		CheckVerticalCollision(motion);

		float sped = speed;


		if (strafeVelocity > sped / Game::instance->deltaTime)
			strafeVelocity = sped / Game::instance->deltaTime;

		if (strafeVelocity < -sped / Game::instance->deltaTime)
			strafeVelocity = -sped / Game::instance->deltaTime;

		if (forwardVelocity > sped / Game::instance->deltaTime)
			forwardVelocity = sped / Game::instance->deltaTime;

		if (forwardVelocity < -sped / Game::instance->deltaTime)
			forwardVelocity = -sped / Game::instance->deltaTime;

		if (shift)
		{
			forwardVelocity *= 0.9;
			strafeVelocity *= 0.9;
		}


		motion += front * (forwardVelocity * Game::instance->deltaTime);

		motion += glm::normalize(glm::cross(front, up)) * (strafeVelocity * Game::instance->deltaTime);

		if (isCreature)
		{
			if (!tiny)
			{
				CheckCollision(motion, 0.2);

				CheckCollision(motion, 0);
			}

			CheckCollision(motion, 1.8);
		}
		else
		{
			CheckCollision(motion, 0);
		}

		bool wouldHaveFallen = motion.y < blockOnShift.y;

		if ((downVelocity > 0) && shift && !shiftCancel)
		{
			shift = false;
			shiftCancel = true;
		}

		if (wouldHaveFallen && shift)
		{
			motion.y = blockOnShift.y;

			motion.x = lastLastFrameMotion.x;
			motion.z = lastLastFrameMotion.z;
		}

		if (shift && !wouldHaveFallen)
			blockOnShift = glm::vec3((int)motion.x, motion.y, (int)motion.z);

		// check if motion is NaN

		if (motion.x != motion.x || motion.y != motion.y || motion.z != motion.z)
			motion = preMotion;

		position = motion;
		lastLastFrameMotion = lastFrameMotion;
		lastFrameMotion = motion;

		if (forwardVelocity != 0)
			forwardVelocity *= 0.94;
		if (strafeVelocity != 0)
			strafeVelocity *= 0.94;

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

		if (position.y >= topBlock)
		{


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
		else
		{
			shadow->src = shadow->t->spriteSheet.GetUVFlip("entity_shadow_100");
			if (!isOnGround)
				drawShadow = false;
			shadow->UpdateSprite();
		}
	}

	shadow->angle = 90;

	if (drawShadow)
		shadow->Draw();
}

Entity::~Entity() {
    delete shadow;
}
