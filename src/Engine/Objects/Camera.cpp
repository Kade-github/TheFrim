#include "Camera.h"
#include <Game.h>
#include <glad/glad.h>
std::vector<GameObject::VVertex> Camera::CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv)
{
	std::vector<GameObject::VVertex> vertices = {};

	GameObject::VVertex tl = GameObject::VVertex(glm::vec3(position.x, position.y, position.z), glm::vec2(uv.x, uv.y));
	GameObject::VVertex tr = GameObject::VVertex(glm::vec3(position.x + size.x, position.y, position.z + size.z), glm::vec2(uv.x + uv.z, uv.y));
	GameObject::VVertex bl = GameObject::VVertex(glm::vec3(position.x, position.y + size.y, position.z + z), glm::vec2(uv.x, uv.y + uv.w));
	GameObject::VVertex br = GameObject::VVertex(glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z + z), glm::vec2(uv.x + uv.z, uv.y + uv.w));

	vertices.push_back(tr);
	vertices.push_back(tl);
	vertices.push_back(bl);
	vertices.push_back(bl);
	vertices.push_back(br);
	vertices.push_back(tr);

	return vertices;
}

void Camera::DebugDraws()
{
	debug->Bind();

	glm::mat4 model = glm::mat4(1.0f);

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniformMat4f("model", &model[0][0]);

	glBindVertexArray(VAO);

	glDisable(GL_CULL_FACE);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GameObject::VVertex), &vertices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, normal));

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	debug->Unbind();

	Game::instance->shader->Unbind();

	vertices.clear();
}

void Camera::ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal)
{
	for(GameObject::VVertex& v : vertices)
		v.normal = normal;
}

void Camera::DrawDebugCube(glm::vec3 pos)
{
	std::vector<GameObject::VVertex> frontVertices = CreateQuad(pos, glm::vec3(1, 1, 0), 0, glm::vec4(0,0,1.0,1.0));
	std::vector<GameObject::VVertex> backVertices = CreateQuad(pos + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, glm::vec4(0, 0, 1.0, 1.0));
	std::vector<GameObject::VVertex> leftVertices = CreateQuad(pos + glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), 0, glm::vec4(0, 0, 1.0, 1.0));
	std::vector<GameObject::VVertex> rightVertices = CreateQuad(pos, glm::vec3(0, 1, 1), 0, glm::vec4(0, 0, 1.0, 1.0));
	std::vector<GameObject::VVertex> topVertices = CreateQuad(pos + glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1, glm::vec4(0, 0, 1.0, 1.0));
	std::vector<GameObject::VVertex> bottomVertices = CreateQuad(pos + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, glm::vec4(0, 0, 1.0, 1.0));

	ApplyNormal(frontVertices, glm::vec3(0,0, -1.0f));
	ApplyNormal(backVertices, glm::vec3(0, 0, 1.0f));
	ApplyNormal(leftVertices, glm::vec3(1.0f, 0, 0));
	ApplyNormal(rightVertices, glm::vec3(-1.0f, 0, 0));
	ApplyNormal(topVertices, glm::vec3(0, 1.0f, 0));
	ApplyNormal(bottomVertices, glm::vec3(0, -1.0f, 0));

	vertices.insert(vertices.end(), frontVertices.begin(), frontVertices.end());
	vertices.insert(vertices.end(), backVertices.begin(), backVertices.end());
	vertices.insert(vertices.end(), leftVertices.begin(), leftVertices.end());
	vertices.insert(vertices.end(), rightVertices.begin(), rightVertices.end());
	vertices.insert(vertices.end(), topVertices.begin(), topVertices.end());
	vertices.insert(vertices.end(), bottomVertices.begin(), bottomVertices.end());


}

void Camera::SetDirection()
{
	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);

	SetViewMatrix();
}

void Camera::SetViewMatrix()
{
	Game::instance->shader->Bind();
	Game::instance->shader->SetUniformMat4f("view", &GetViewMatrix()[0][0]);
	Game::instance->shader->Unbind();
}

Camera::Camera()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	debug = Texture::createWithImage("Assets/Textures/SelectionCube.png");
	debug->dontDelete = true;
}

Camera::~Camera()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	delete debug;
}
