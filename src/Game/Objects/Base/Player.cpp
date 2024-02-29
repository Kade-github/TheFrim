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

	static float lastPitch = 0;
	static float lastYaw = 0;
	static glm::vec3 lastPos = glm::vec3(0, 0, 0);

	camera->pitch = p;
	camera->yaw = yaw;

	if (lastPitch != p || lastYaw != yaw || lastPos != camera->position)
	{
		camera->SetDirection();
		lastPitch = p;
		lastYaw = yaw;
		lastPos = camera->position;
	}
}

glm::vec3 Player::ForwardRay()
{
	Camera* camera = Game::instance->GetCamera();

	glm::vec3 ray = position;

	Chunk* chunk = WorldManager::instance->GetChunk(position.x, position.z);

	float rX = (position.x - chunk->position.x);
	float rZ = (position.z - chunk->position.z);

	bool hit = chunk->doesBlockExist(rX, ray.y, rZ);

	glm::vec3 end = camera->cameraFront * 5.0f;

	float progress = 0;

	while (progress < 1)
	{
		ray = position + (end * progress);

		rX = (ray.x - chunk->position.x);
		rZ = (ray.z - chunk->position.z);

		hit = chunk->doesBlockExist(rX, ray.y, rZ) >= 0;
		if (hit)
			break;

		progress += 0.1;
	}

	return glm::vec3((int)ray.x, (int)ray.y, (int)ray.z);
	
}

void Player::MouseClick(int button, glm::vec2 mPos)
{

}
