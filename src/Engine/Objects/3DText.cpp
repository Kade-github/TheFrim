#include "3DText.h"
#include <Game.h>

Text3D::Text3D(std::string text, std::string font, glm::vec3 pos, glm::vec4 color, int size) : GameObject(pos)
{
	_textRenderer = new Text2D(text, font, glm::vec3(0,0,0), color, size);
	_camera = new Camera2D(glm::vec3(0, 0, 0));
	_camera->s = new Shader();
	_camera->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	_camera->AddObject(_textRenderer);


	// init stuff

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	UpdateText(text);
}

void Text3D::UpdateText(std::string text)
{
	_textRenderer->text = text;
	_textRenderer->Draw();

	_camera->_rW = _textRenderer->width;
	_camera->_rH = _textRenderer->height;
	_camera->_w = _textRenderer->width;
	_camera->_h = _textRenderer->height;

	_camera->ResizeTo();

	_camera->UpdateFramebuffer();

	vertices.clear();

	for (Vertex2D& v : _camera->vertices)
	{
		VVertex vv;
		vv.position = glm::vec3(v.x, v.y, 0);
		vv.uv = glm::vec2(v.u, v.v);

		vertices.push_back(vv);
	}

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

void Text3D::Draw()
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

	_camera->t->Bind();

	glDisable(GL_CULL_FACE);

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);

	_camera->t->Unbind();

	Game::instance->shader->Unbind();
}

void Text3D::Destroy()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	delete _textRenderer;
	delete _camera;
}