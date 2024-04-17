#include "BlockTest.h"
#include <Game.h>

#include "../Objects/Base/Blocks/Grass.h"
#include "../Objects/Base/Blocks/Dirt.h"
#include "../Objects/Base/Blocks/Stone.h"
#include "../Objects/Base/Blocks/Cobblestone.h"
#include "../Objects/Base/Blocks/Wood.h"
#include "../Objects/Base/Blocks/Leaves.h"
#include "../Objects/Base/Blocks/Sand.h"
#include "../Objects/Base/Blocks/Water.h"
#include "../Objects/Base/Blocks/CraftingTable.h"
#include "../Objects/Base/Blocks/WoodenPlanks.h"

bool firstMouse = false;

float p = 0;
float yaw = 0;
float lastX = 400, lastY = 300;

BlockTest::BlockTest()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	t = Texture::createWithImage("Assets/Textures/block.png");

}

void BlockTest::ChangeBlock()
{
	Block* block = nullptr;

	switch (blockId)
	{
	case GRASS:
		block = new Grass(glm::vec3());
		break;
	case STONE:
		block = new Stone(glm::vec3());
		break;
	case COBBLESTONE:
		block = new Cobblestone(glm::vec3());
		break;
	case WOOD:
		block = new Wood(glm::vec3());
		break;
	case LEAVES:
		block = new Leaves(glm::vec3());
		break;
	case SAND:
		block = new Sand(glm::vec3());
		break;
	case WATER:
		block = new Water(glm::vec3());
		break;
	case CRAFTINGTABLE:
		block = new CraftingTable(glm::vec3());
		break;
	case WOODENPLANKS:
		block = new WoodenPlank(glm::vec3());
		break;
	default:
		block = new Dirt(glm::vec3());
		break;
	}

	block->t = t;

	BlockFace ff = block->CreateFrontFace();
	block->faces.push_back(ff);
	BlockFace bf = block->CreateBackFace();
	block->faces.push_back(bf);
	BlockFace lf = block->CreateLeftFace();
	block->faces.push_back(lf);
	BlockFace rf = block->CreateRightFace();
	block->faces.push_back(rf);
	BlockFace tf = block->CreateTopFace();
	block->faces.push_back(tf);
	BlockFace botf = block->CreateBottomFace();
	block->faces.push_back(botf);

	vertices.clear();
	indices.clear();

	for (BlockFace f : block->faces)
	{
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
	}

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GameObject::VVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void BlockTest::Draw()
{
	Shader* s = Game::instance->shader;

	s->Bind();

	t->Bind();

	Camera* cam = Game::instance->GetCamera();

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
	p += yoffset;
	if (p >= 89.0f)
		p = 89.0f;
	else if (p <= -89.0f)
		p = -89.0f;

	yaw += xoffset;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
		cam->position += cam->cameraFront * 0.05f;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
		cam->position -= cam->cameraFront * 0.05f;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
		cam->position -= glm::normalize(glm::cross(cam->cameraFront, cam->cameraUp)) * 0.05f;

	if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
		cam->position += glm::normalize(glm::cross(cam->cameraFront, cam->cameraUp)) * 0.05f;

	cam->pitch = p;
	cam->yaw = yaw;

	cam->SetDirection();

	glBindVertexArray(VAO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	t->Unbind();

	s->Unbind();

	Scene::Draw();
}

void BlockTest::KeyPress(int key)
{
}