#include "TestScene.h"
#include <imgui.h>

#include <filesystem>

#include <Objects/2DCamera.h>


float deltaTime = 0.0f;	// Time between current frame and last frame
float lastFrame = 0.0f; // Time of last frame

float cameraSpeed = 10.0f;

bool firstMouse = false;

float lastX = 400, lastY = 300;


void TestScene::Create()
{
	if (!std::filesystem::exists("worlds"))
		std::filesystem::create_directory("worlds");

	wm = new WorldManager("worlds/test", Texture::createWithImage("Assets/Textures/sheet.png"));

	Camera* camera = Game::instance->GetCamera();

	camera->position = glm::vec3(0.0f, 129.0f, 0.0f);

	Camera2D* cam = new Camera2D(glm::vec3(0,0,0));

	Shader* Shader2D = new Shader();
	Shader2D->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	cam->s = Shader2D;

	AddObject(cam);
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
		camera->position += camera->cameraFront * deltaTime * cameraSpeed;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		camera->position -= camera->cameraFront * deltaTime * cameraSpeed;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		camera->position -= glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * deltaTime * cameraSpeed;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		camera->position += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * deltaTime * cameraSpeed;


	ImGui::Begin("Debug", 0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("FPS: %.2f", ImGui::GetIO().Framerate);

	ImGui::Text("Camera Pos: %f, %f, %f", camera->position.x, camera->position.y, camera->position.z);

	ImGui::SliderFloat("Render Distance", &camera->cameraFar, 0.1f, 400.0f);

	ImGui::SliderFloat("Camera Speed", &cameraSpeed, 1.0f, 100.0f);

	ImGui::End();

	wm->RenderChunks();


	Scene::Draw();
}

void TestScene::Destroy()
{
	wm->SaveWorldNow();
}
