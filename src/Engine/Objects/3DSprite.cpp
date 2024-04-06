#include "3DSprite.h"
#include <Game.h>

Sprite3D::Sprite3D(std::string path, glm::vec3 pos) : GameObject(pos)
{
	t = Texture::createWithImage(path, false);
	// init stuff

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	UpdateSprite();
}

void Sprite3D::Draw()
{
	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), rotateAxis);

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniformMat4f("model", &model[0][0]);

	t->Bind();

	glDisable(GL_CULL_FACE);
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);

	t->Unbind();

	Game::instance->shader->Unbind();
}

void Sprite3D::UpdateSprite()
{
	vertices.clear();

	vertices.push_back({ glm::vec3(-width / 2, -height / 2, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(width / 2, -height / 2, 0), glm::vec2(1, 0) });
	vertices.push_back({ glm::vec3(width / 2, height / 2, 0), glm::vec2(1, 1) });

	vertices.push_back({ glm::vec3(-width / 2, -height / 2, 0), glm::vec2(0, 0) });
	vertices.push_back({ glm::vec3(width / 2, height / 2, 0), glm::vec2(1, 1) });
	vertices.push_back({ glm::vec3(-width / 2, height / 2, 0), glm::vec2(0, 1) });


	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VVertex), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)offsetof(VVertex, uv));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Sprite3D::Destroy()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
}
