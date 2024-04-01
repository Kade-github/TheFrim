#include "../../Scenes/Gameplay.h"
#include <Game.h>
#include <glm/gtx/rotate_vector.hpp>
#include "../../LightingManager.h"

bool firstMouse = false;

float lastX = 400, lastY = 300;

void Player::ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal)
{
	for (GameObject::VVertex& v : vertices)
		v.normal = normal;
}

void Player::Hurt(float damage)
{
	playerData.health -= damage;

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->UpdateHearts();

	CameraShake(0.4f);
}

void Player::Heal(float amount)
{
	playerData.health += amount;

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->UpdateHearts();
}

void Player::CameraShake(float amount)
{
	_shake = amount;
}

void Player::DrawBlockBreak(BlockFace f)
{
	vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());

	for (int i = 0; i < f.indices.size(); i++)
		indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
}

void Player::SetBreakBuffer()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GameObject::VVertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}

void Player::RenderBreak()
{
	SetBreakBuffer();

	WorldManager* wm = WorldManager::instance;

	Shader* shader = Game::instance->shader;

	shader->Bind();
	wm->texturePack->Bind();

	glm::mat4 model = glm::mat4(1.0f);

	shader->SetUniformMat4f("model", &model[0][0]);

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	wm->texturePack->Unbind();
	shader->Unbind();
}

Player::Player(glm::vec3 pos) : Entity(pos)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
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

	bool moving = false;

	if (!freeCam)
	{

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		{
			forwardVelocity += speed;
			headBop += 10.0f * Game::instance->deltaTime;
			moving = true;
		}

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		{
			forwardVelocity -= speed;
			headBop += 10.0f * Game::instance->deltaTime;
			moving = true;
		}

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		{
			strafeVelocity -= speed;
			headBop += 10.0f * Game::instance->deltaTime;
			moving = true;
		}

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		{
			strafeVelocity += speed;
			headBop += 10.0f * Game::instance->deltaTime;
			moving = true;
		}

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && isOnGround)
		{
			if (glfwGetTime() - jumpCooldown < 0.15)
				return;

			jumpCooldown = glfwGetTime();

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

	if (headBop > 0)
	{
		static bool footstepped = false;
		if (!moving || !isOnGround)
			headBop -= 20.0f * Game::instance->deltaTime;

		if (headBop > 1.25f)
			headBop = 1.25f;

		float s = (sin(glfwGetTime() * 16) * 0.05f) * headBop;

		camera->position.y += s;

		if (s < -0.03 && !footstepped)
		{
			Footstep();
			footstepped = true;
		}

		if (s > 0.03)
			footstepped = false;
	}


	camera->pitch = p;
	camera->yaw = yaw;

	if (_shake > 0)
	{
		_shake -= 1.75f * Game::instance->deltaTime;
		camera->position.x += ((rand() % 100) / 100.0f) * _shake;
		camera->position.y += ((rand() % 100) / 100.0f) * _shake;
		camera->position.z += ((rand() % 100) / 100.0f) * _shake;
	}

	camera->SetDirection();


	glm::vec3 ray = position + (camera->cameraFront * 5.0f);
	bool hit = RayTo(ray, true);
	if (hit)
	{
		Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

		if (c != nullptr)
		{
			subChunk& sb = c->GetSubChunk(ray.y);

			glm::vec3 _world = c->WorldToChunk(ray);

			selectedBlock = sb.getBlock(_world.x, _world.z);
		}
	}

	vertices.clear();
	indices.clear();

	if (selectedBlock != nullptr && glfwGetMouseButton(Game::instance->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		selectedBlock->breakProgress += (1.0f * Game::instance->deltaTime) * selectedBlock->toughness;

		if (selectedBlock->breakProgress >= 1)
		{
			selectedBlock->breakProgress = 0;

			glm::vec3 _world = selectedBlock->position;

			Chunk* c = WorldManager::instance->GetChunk(_world.x, _world.z);

			if (c != nullptr)
			{
				c->ModifyBlock(_world.x, _world.y, _world.z, 0);
			}
		}

		std::vector<BlockFace> faces = {};

		faces.push_back(selectedBlock->BreakTopFace());
		ApplyNormal(faces[0].vertices, glm::vec3(0, 1, 0));
		faces.push_back(selectedBlock->BreakBottomFace());
		ApplyNormal(faces[1].vertices, glm::vec3(0, -1, 0));
		faces.push_back(selectedBlock->BreakLeftFace());
		ApplyNormal(faces[2].vertices, glm::vec3(1, 0, 0));
		faces.push_back(selectedBlock->BreakRightFace());
		ApplyNormal(faces[3].vertices, glm::vec3(-1, 0, 0));
		faces.push_back(selectedBlock->BreakFrontFace());
		ApplyNormal(faces[4].vertices, glm::vec3(0, 0, -1));
		faces.push_back(selectedBlock->BreakBackFace());
		ApplyNormal(faces[5].vertices, glm::vec3(0, 0, 1));

		for (int i = 0; i < faces.size(); i++)
		{
			BlockFace f = faces[i];

			for(int j = 0; j < f.vertices.size(); j++)
				f.vertices[j].position += f.vertices[j].normal * 0.01f;

			DrawBlockBreak(f);
		}

		RenderBreak();
	}
	else if (selectedBlock != nullptr)
	{
		selectedBlock->breakProgress = 0;
	}
}

void Player::MouseClick(int button, glm::vec2 mPos)
{
	Camera* camera = Game::instance->GetCamera();

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

			if (c->DoesBlockExist(x, y, z))
				return;

			if ((int)x == (int)position.x && (int)z == (int)position.z && (y == (int)position.y || y == (int)position.y - 1))
				return;

			if (c != nullptr)
			{
				// place block
			}
		}
	}
}

void Player::KeyPress(int key)
{
	if (key == GLFW_KEY_F1) // place light
		LightingManager::GetInstance()->AddLight(glm::vec3((int)position.x, (int)position.y + 2, (int)position.z), 12);

	if (key == GLFW_KEY_F2) // remove light
		LightingManager::GetInstance()->RemoveLight(glm::vec3((int)position.x, (int)position.y + 2, (int)position.z));

	if (key == GLFW_KEY_F8) // hurt
	{
		Hurt(0.5f);
	}

	if (key == GLFW_KEY_F9) // heal
	{
		Heal(0.5f);
	}
}

Player::~Player()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}