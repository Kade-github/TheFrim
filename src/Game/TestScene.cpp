#include "TestScene.h"

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse = false;

float lastX = 400, lastY = 300;



void TestScene::Draw()
{
	float currentFrame = glfwGetTime();
	deltaTime = currentFrame - lastFrame;
	lastFrame = currentFrame;

	glm::vec2 mousePos = Game::instance->GetCursorPos();

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

	float p = camera->pitch + yoffset;
	if (p >= 89.0f)
		p = 89.0f;
	else if (p <= -89.0f)
		p = -89.0f;

	camera->yaw += xoffset;
	camera->pitch = p;

	camera->SetDirection();

	// camera movement

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		camera->position += camera->cameraFront * deltaTime * 10.0f;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		camera->position -= camera->cameraFront * deltaTime * 10.0f;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		camera->position -= glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * deltaTime * 10.0f;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		camera->position += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * deltaTime * 10.0f;


	Scene::Draw();
}
