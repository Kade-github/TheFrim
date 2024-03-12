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

	if (!freeCam)
	{

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
			if (jumpCooldown < glfwGetTime())
			{
				jumpCooldown = glfwGetTime() + 0.15f;
				downVelocity = jumpStrength;
				isOnGround = false;
			}
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
	}
	else
	{
		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
			position += camera->cameraFront * 0.05f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
			position -= camera->cameraFront * 0.05f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
			position -= glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * 0.05f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
			position += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * 0.05f;
	}

	glm::vec3 storedPos = position;

	camera->position = storedPos;
	camera->position.y -= 0.1;

	camera->pitch = p;
	camera->yaw = yaw;

	camera->SetDirection();

}

void Player::MouseClick(int button, glm::vec2 mPos)
{
	Camera* camera = Game::instance->GetCamera();

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		glm::vec3 ray = position + (camera->cameraFront * 5.0f);
		bool hit = RayTo(ray, true);
		if (hit)
		{
			Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

			if (c != nullptr)
				c->ModifyBlock(ray.x, ray.y, ray.z, 0);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		glm::vec3 ray = position + (camera->cameraFront * 5.0f);
		bool hit = RayTo(ray);
		if (hit)
		{
			float x = ray.x;
			int y = (int)ray.y;
			float z = ray.z;

			Chunk* c = WorldManager::instance->GetChunk(x, z);

			if (c->DoesBlockExist(x,y,z))
				return;

			if ((int)x == (int)position.x && (int)z == (int)position.z && (y == (int)position.y || y == (int)position.y - 1))
				return;

			if (c != nullptr)
				c->ModifyBlock(x,y,z, BlockType::COBBLESTONE);
		}
	}
}
