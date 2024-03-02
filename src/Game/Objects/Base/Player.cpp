#include "Player.h"
#include <Game.h>
#include "../../WorldManager.h"

bool firstMouse = false;

float lastX = 400, lastY = 300;

Player::Player(glm::vec3 pos) : Entity(pos)
{
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

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		forwardVelocity += speed;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		forwardVelocity -= speed;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		strafeVelocity -= speed;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		strafeVelocity += speed;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && isOnGround)
	{
		downVelocity = jumpStrength;
		isOnGround = false;
	}

	if (forwardVelocity > speed)
		forwardVelocity = speed;

	if (forwardVelocity < -speed)
		forwardVelocity = -speed;

	if (strafeVelocity > speed)
		strafeVelocity = speed;

	if (strafeVelocity < -speed)
		strafeVelocity = -speed;


	Entity::Draw(); // physics

	camera->position = position;

	camera->pitch = p;
	camera->yaw = yaw;

	camera->SetDirection();
}

glm::vec3 Player::ForwardRay()
{
	Camera* camera = Game::instance->GetCamera();

	glm::vec3 ray = position;

	Chunk* chunk = WorldManager::instance->GetChunk(position.x, position.z);

	Block* hit = chunk->GetBlock(position.x, position.y, position.z);

	glm::vec3 end = camera->cameraFront * 5.0f;

	float progress = 0;

	while (progress < 1)
	{
		ray = position + (end * progress);

		chunk = WorldManager::instance->GetChunk(ray.x, ray.z);

		hit = chunk->GetBlock(ray.x, ray.y, ray.z);
		if (hit != nullptr)
		{
			ray.y = hit->position.y;
			break;
		}

		progress += 0.05;
	}

	return glm::vec3((int)ray.x, (int)ray.y, (int)ray.z);
	
}

void Player::MouseClick(int button, glm::vec2 mPos)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		glm::vec3 ray = ForwardRay();

		// TODO: Add block breaking
	}
}
