#include "TestScene.h"
#include "../Engine/Game.h"

#include "Objects/Base/Blocks/Grass.h"
#include "Objects/Base/Blocks/Dirt.h"
#include "Objects/Base/Chunk.h"

float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

bool firstMouse = false;

float lastX = 400, lastY = 300;

void TestScene::Create()
{
	Texture* t = Texture::createWithImage("Assets/Textures/sheet.png");

	// 1 chunk 16x256x16

	Chunk* chunk = new Chunk(glm::vec3(0, 0, 0), t);

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y > -256; y--)
		{
			for (int z = 0; z < 16; z++)
			{
				if (y < 0)
					chunk->AddBlock(new Dirt(glm::vec3(x, y, z)));
				else
					chunk->AddBlock(new Grass(glm::vec3(x, y, z)));
			}
		}
	}

	chunk->GenerateMesh();

	AddObject(chunk);

}

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

	Camera* camera = Game::instance->GetCamera();

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

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_F2) == GLFW_PRESS)
	{
		Chunk* c = (Chunk*)objects[0];
		std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
		c->GenerateMesh();
		std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

		std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "[ms]" << std::endl;
	}


	Scene::Draw();
}
