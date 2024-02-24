#include "2DCamera.h"
#include <Game.h>
#include <imgui.h>
#include <Helpers/StringTools.h>

Camera2D::Camera2D(glm::vec3 pos) : GameObject(pos)
{
	debugText = new Text2D("", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1));

	_w = Game::instance->GetWindowSize().x;
	_h = Game::instance->GetWindowSize().y;

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

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, u));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(offsetof(Vertex2D, r)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &s_vao);

	glBindVertexArray(s_vao);

	glGenBuffers(1, &s_vbo);


	Vertex2D tl = { glm::vec3{0,0,0} , glm::vec2{0,0}, glm::vec4(1,1,1,1)};
	Vertex2D tr = { glm::vec3{_w,0,0} , glm::vec2{1,0}, glm::vec4(1,1,1,1)};
	Vertex2D bl = { glm::vec3{0,_h,0} , glm::vec2{0,1}, glm::vec4(1,1,1,1)};
	Vertex2D br = { glm::vec3{_w,_h,0} , glm::vec2{1,1}, glm::vec4(1,1,1,1)};

	vertices.push_back(tl);
	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(br);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, u));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(offsetof(Vertex2D, r)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);


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

void Camera2D::DrawDebugText(std::string text, glm::vec2 pos, int size)
{
	debugText->text = text;
	debugText->size = size;
	debugText->position = glm::vec3(pos, 0);
	debugText->color = glm::vec4(1, 1, 1, 1);
	debugText->Draw();
	dbgDraws.push_back(debugText->draws[0]);
}

void Camera2D::Draw()
{
	if (s == NULL)
		return;

	dbgDraws.clear();

	float fps = ImGui::GetIO().Framerate;

	std::string format = StringTools::ToTheDecimial(fps, 0);

	DrawDebugText("FPS: " + format, glm::vec2(4, _h - 28), 24);

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
			for (int k = 0; k < draws.size(); k++)
			{
				if (draws[k].textureId == draw.textureId && draws[k].shaderId == draw.shaderId)
				{
					for (int l = 0; l < draw.vertices.size(); l++)
						draws[k].vertices.push_back(draw.vertices[l]);
					break;
				}
			}
			draws.push_back(draw);
		}
	}

	for(int i = 0; i < dbgDraws.size(); i++)
	{
		Draw2D draw = dbgDraws[i];
		for (int k = 0; k < draws.size(); k++)
		{
			if (draws[k].textureId == draw.textureId && draws[k].shaderId == draw.shaderId)
			{
				for (int l = 0; l < draw.vertices.size(); l++)
					draws[k].vertices.push_back(draw.vertices[l]);
				break;
			}
		}
		draws.push_back(draw);
	}

	s->Bind();

	glm::mat4 projection = glm::ortho(0.0f, _w, _h, 0.0f);
	s->SetUniformMat4f("projection", &projection[0][0]);

	glDisable(GL_DEPTH_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, fb);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glViewport(0,0, _w, _h);


	for (int i = 0; i < draws.size(); i++)
	{
		Draw2D draw = draws[i];
		if (draw.shaderId == NULL)
			s->Bind();
		else
		{
			draw.shaderId->Bind();
			draw.shaderId->SetUniformMat4f("projection", &projection[0][0]);
		}

		if (draw.textureId != NULL)
			draw.textureId->Bind();

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, draw.vertices.size() * sizeof(Vertex2D), draw.vertices.data(), GL_STATIC_DRAW);

		glDrawArrays(GL_TRIANGLES, 0, draw.vertices.size());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		if (draw.textureId != NULL)
			draw.textureId->Unbind();

		if (draw.shaderId != NULL)
			draw.shaderId->Unbind();
		else
			s->Unbind();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glViewport(0, 0, Game::instance->GetWindowSize().x, Game::instance->GetWindowSize().y);

	s->Bind();

	if (t != NULL)
		t->Bind();

	glBindVertexArray(s_vao);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);

	if (t != NULL)
		t->Unbind();

	s->Unbind();

	glEnable(GL_DEPTH_TEST);

}
