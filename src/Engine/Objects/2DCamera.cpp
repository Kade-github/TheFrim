#include "2DCamera.h"
#include <Game.h>

Camera2D::Camera2D(glm::vec3 pos) : GameObject(pos)
{
	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

	_w = Game::instance->GetWindowSize().x;
	_h = Game::instance->GetWindowSize().y;

	t = new Texture(NULL, _w, _h);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t->id, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		Game::instance->log->Write("[2DCamera] Framebuffer is not complete!");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenBuffers(1, &vbo);
	glGenVertexArrays(1, &vao);

	glGenBuffers(1, &s_vbo);
	glGenVertexArrays(1, &s_vao);

}

Camera2D::~Camera2D()
{
	glDeleteFramebuffers(1, &fb);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &s_vbo);
	glDeleteVertexArrays(1, &s_vao);
}

void Camera2D::Resize()
{
	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	t->resizeTexture(Game::instance->GetWindowSize().x, Game::instance->GetWindowSize().y);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, t->id, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Camera2D::Draw()
{
	if (s == NULL)
		return;

	Camera* c = Game::instance->GetCamera();

	glm::vec3 realPos = c->position + c->cameraFront;

	std::vector<Draw2D> draws = {};

	for (int i = 0; i < objects.size(); i++)
	{
		GameObject2D* object = objects[i];
		object->Draw();

		for (int j = 0; j < object->draws.size(); j++)
		{
			Draw2D draw = object->draws[j];
			// check if the texture and shader are already in there
			bool found = false;
			for (int k = 0; k < draws.size(); k++)
			{
				if (draws[k].textureId == draw.textureId && draws[k].shaderId == draw.shaderId)
				{
					found = true;
					for (int l = 0; l < draw.vertices.size(); l++)
					{
						draws[k].vertices.push_back(draw.vertices[l]);
					}
					break;
				}
			}
			draws.push_back(draw);
		}
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);

	glViewport(0,0, _w, _h);

	glm::mat4 projection = glm::ortho(0.0f, _w, _h, 0.0f, -1.0f, 1.0f);

	for (int i = 0; i < draws.size(); i++)
	{
		Draw2D draw = draws[i];
		if (draw.textureId == NULL)
		{
			s->Bind();
			s->SetUniformMat4f("projection", &projection[0][0]);
		}
		else
		{
			draw.shaderId->Bind();
			draw.shaderId->SetUniformMat4f("projection", &projection[0][0]);
		}

		if (draw.textureId != NULL)
			draw.textureId->Bind();

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, draw.vertices.size() * sizeof(Vertex2D), &draw.vertices[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
		glEnableVertexAttribArray(0);

		// uv attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, texCoords));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_TRIANGLES, 0, draw.vertices.size());

		glBindVertexArray(0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Shader* baseShader = Game::instance->shader;

	baseShader->Bind();

	t->Bind();

	glBindVertexArray(s_vao);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

	vertices = {};

	glm::vec3 cameraTopLeft = glm::inverse(c->GetViewMatrix()) * glm::vec4(-1, 1, 0, 1);
	glm::vec3 cameraBottomRight = glm::inverse(c->GetViewMatrix()) * glm::vec4(1, -1, 0, 1);

	vertices.push_back(Cam::Vertex(cameraTopLeft, glm::vec2(0, 0))); // tl
	vertices.push_back(Cam::Vertex(glm::vec3(cameraTopLeft.x, cameraBottomRight.y, cameraTopLeft.z), glm::vec2(0, 1))); // bl
	vertices.push_back(Cam::Vertex(glm::vec3(cameraBottomRight.x, cameraTopLeft.y, cameraTopLeft.z), glm::vec2(1, 0))); // tr
	vertices.push_back(Cam::Vertex(glm::vec3(cameraBottomRight.x, cameraTopLeft.y, cameraTopLeft.z), glm::vec2(1, 0))); // tr
	vertices.push_back(Cam::Vertex(glm::vec3(cameraTopLeft.x, cameraBottomRight.y, cameraTopLeft.z), glm::vec2(0, 1))); // bl
	vertices.push_back(Cam::Vertex(cameraBottomRight, glm::vec2(1, 1))); // br

	baseShader->SetUniformMat4f("view", &c->GetViewMatrix()[0][0]);
	baseShader->SetUniformMat4f("projection", &c->GetProjectionMatrix()[0][0]);

	glm::mat4 mtx_trans = glm::mat4(1.0f);

	baseShader->SetUniformMat4f("model", &mtx_trans[0][0]);

	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Cam::Vertex), &vertices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Cam::Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Cam::Vertex), (void*)offsetof(Cam::Vertex, uv));
	glEnableVertexAttribArray(1);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);

}
