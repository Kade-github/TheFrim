#include "Player.h"
#include <Game.h>
#include "../../WorldManager.h"

bool firstMouse = false;

float lastX = 400, lastY = 300;

Player::Player(glm::vec3 pos) : GameObject(pos)
{
}

void Player::CheckCollision(glm::vec3& motion)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	if (currentChunk != NULL)
	{
		Block* b = currentChunk->getBlock(motion.x, motion.y - 2, motion.z);

		glm::vec3 diff = motion - position;

		if (diff.x == 0 && diff.y == 0 && diff.z == 0)
			return;

		if (b != NULL)
		{
			glm::vec3 nonCollidedPos = motion;

			Block* b2 = b;
			Block* b3 = b;

			while (b != NULL || b2 != NULL || b3 != NULL)
			{
				b = currentChunk->getBlock(nonCollidedPos.x, nonCollidedPos.y - 2, position.z);
				if (b != NULL)
					nonCollidedPos.x -= glm::normalize(diff).x * 0.001f;

				b2 = currentChunk->getBlock(position.x, nonCollidedPos.y - 2, nonCollidedPos.z);
				if (b2 != NULL)
					nonCollidedPos.z -= glm::normalize(diff).z * 0.001f;

				b3 = currentChunk->getBlock(nonCollidedPos.x, nonCollidedPos.y - 2, nonCollidedPos.z);
				if (b3 != NULL)
					nonCollidedPos -= glm::normalize(diff) * 0.001f;

			}

			motion = nonCollidedPos;
		}
	}
}

void Player::CheckVerticalCollision(glm::vec3& motion)
{
	Chunk* currentChunk = WorldManager::instance->GetChunk(motion.x, motion.z);

	isOnGround = false;

	if (currentChunk != NULL)
	{
		float rY = motion.y - 2;

		if (downVelocity > 0)
			rY = motion.y + 3;

		Block* b = currentChunk->getBlock(motion.x, rY, motion.z);

		while (b != NULL)
		{
			motion.y = b->position.y + 3;
			if (downVelocity > 0)
				motion.y = b->position.y - 3;
			if (downVelocity < 0)
				isOnGround = true;
			downVelocity = 0;

			rY = motion.y - 2;

			b = currentChunk->getBlock(motion.x, rY, motion.z);
		}
	}
}

glm::vec3 Player::Ray()
{
	glm::vec3 ray = Game::instance->GetCamera()->cameraFront;

	glm::vec3 pos = position;

	for (int i = 0; i < 100; i++)
	{
		pos += ray * 0.1f;

		Chunk* currentChunk = WorldManager::instance->GetChunk(pos.x, pos.z);

		if (currentChunk != NULL)
		{
			Block* b = currentChunk->getBlock(pos.x, pos.y, pos.z);

			if (b != NULL)
			{
				return pos;
			}
		}
	}

	return glm::vec3(0, 0, 0);
	
}

void Player::Draw()
{
	glm::vec2 mousePos = Game::instance->GetCursorPos(false);

	float x = mousePos.x;
	float y = mousePos.y;

	if (firstMouse)
	{
		lastX = x;
		lastY = y;
		firstMouse = false;
	}

	float xoffset = x - lastX;
	float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

	lastX = x;
	lastY = y;

	const float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	Camera* camera = Game::instance->GetCamera();

	float p = camera->pitch + yoffset;
	if (p >= 89.0f)
		p = 89.0f;
	else if (p <= -89.0f)
		p = -89.0f;

	yaw += xoffset;

	SetDirection();

	// camera movement

	bool forwardKey = false;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
	{
		forwardKey = true;
		forwardVelocity += 6;
	}

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
	{
		forwardKey = true;
		forwardVelocity -= 6;
	}

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
	{
		strafeVelocity -= 6;
	}

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
	{
		strafeVelocity += 6;
	}

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		if (isOnGround)
			downVelocity = 400.0f;
	}

	strafeVelocity *= Game::instance->deltaTime;
	forwardVelocity *= Game::instance->deltaTime;

	glm::vec3 motion = position;

	motion += front * forwardVelocity;

	motion += glm::normalize(glm::cross(front, up)) * strafeVelocity;


	CheckCollision(motion);
	
	position = motion;

	if (forwardVelocity > playerSpeed * 0.8)
		forwardVelocity = playerSpeed * 0.8;

	if (forwardVelocity < -playerSpeed * 0.8)
		forwardVelocity = -playerSpeed * 0.8;

	if (isOnGround)
	{
		if (strafeVelocity > playerSpeed * 0.8)
			strafeVelocity = playerSpeed * 0.8;

		if (strafeVelocity < -playerSpeed * 0.8)
			strafeVelocity = -playerSpeed * 0.8;
	}
	else
	{
		if (forwardKey)
		{
			if (strafeVelocity > playerSpeed * 0.8)
				strafeVelocity = playerSpeed * 0.8;

			if (strafeVelocity < -playerSpeed * 0.8)
				strafeVelocity = -playerSpeed * 0.8;
		}
		else
		{
			if (strafeVelocity > playerSpeed)
				strafeVelocity = playerSpeed;

			if (strafeVelocity < -playerSpeed)
				strafeVelocity = -playerSpeed;
		}
	}

	forwardVelocity *= 0.8;
	strafeVelocity *= 0.8;

	// gravity

	glm::vec3 _to = position;
	downVelocity -= 8.0f;

	downVelocity *= Game::instance->deltaTime;

	_to.y += downVelocity;
	
	CheckVerticalCollision(_to);

	position.y = _to.y;

	if (downVelocity < -18)
		downVelocity = -18;

	camera->position = position;

	camera->pitch = p;
	camera->yaw = yaw;

	camera->SetDirection();
}
