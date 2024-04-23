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

Sprite3D::Sprite3D(Texture* _t, glm::vec3 pos) : GameObject(pos)
{
	t = _t;
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

	// angleZ

	model = glm::rotate(model, glm::radians(angleZ), glm::vec3(0, 0, 1));

	// angleY

	model = glm::rotate(model, glm::radians(angleY), glm::vec3(0, 1, 0));

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniformMat4f("model", &model[0][0]);

	t->Bind();

	if (!depth)
		glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);
	if (!depth)
		glEnable(GL_DEPTH_TEST);

	t->Unbind();

	Game::instance->shader->Unbind();
}

void Sprite3D::UpdateSprite()
{
	vertices.clear();


	glm::vec3 _tl = glm::vec3(-width / 2, 0, 0);
	glm::vec3 _tr = glm::vec3(width / 2, 0, 0);
	glm::vec3 _br = glm::vec3(width / 2, height, 0);
	glm::vec3 _bl = glm::vec3(-width / 2, height, 0);

	VVertex tl, tr, br, bl;

	tl.position = _tl;
	tr.position = _tr;
	br.position = _br;
	bl.position = _bl;

	float rX = t->width * src.x;
	float rY = t->height * src.y;
	float rW = t->width * src.z;
	float rH = t->height * src.w;
	
	tl.uv = glm::vec2(rX / t->width, rY / t->height);
	tr.uv = glm::vec2((rX + rW) / t->width, rY / t->height);
	br.uv = glm::vec2((rX + rW) / t->width, (rY + rH) / t->height);
	bl.uv = glm::vec2(rX / t->width, (rY + rH) / t->height);

	vertices.push_back(tl);
	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(br);

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
