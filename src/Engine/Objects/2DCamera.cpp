#include "2DCamera.h"
#include <Game.h>
#include <imgui.h>
#include <Helpers/StringTools.h>

Camera2D::Camera2D(glm::vec3 pos) : GameObject(pos)
{
	debugText = new Text2D("", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1));

	_w = 1920;
	_h = 1080;

	_rW = 1920;
	_rH = 1080;

	def = Texture::createWithImage("Assets/Textures/Pixel.png");
	def->dontDelete = true;

	glGenFramebuffers(1, &fb);
	glBindFramebuffer(GL_FRAMEBUFFER, fb);

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


	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, u));
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)(offsetof(Vertex2D, r)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	Vertex2D tl = { glm::vec3{0,0,0} , glm::vec2{0,0}, glm::vec4(1,1,1,1) };
	Vertex2D tr = { glm::vec3{_rW,0,0} , glm::vec2{1,0}, glm::vec4(1,1,1,1) };
	Vertex2D bl = { glm::vec3{0,_rH,0} , glm::vec2{0,1}, glm::vec4(1,1,1,1) };
	Vertex2D br = { glm::vec3{_rW,_rH,0} , glm::vec2{1,1}, glm::vec4(1,1,1,1) };

	vertices.push_back(tl);
	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(br);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Camera2D::Resize()
{

	glm::vec2 size = Game::instance->GetWindowSize();

	_rW = size.x;
	_rH = size.y;

	ResizeTo();
}

void Camera2D::ResizeTo()
{
	vertices.clear();

	Vertex2D tl = { glm::vec3{0,0,0} , glm::vec2{0,0}, glm::vec4(1,1,1,1) };
	Vertex2D tr = { glm::vec3{_rW,0,0} , glm::vec2{1,0}, glm::vec4(1,1,1,1) };
	Vertex2D bl = { glm::vec3{0,_rH,0} , glm::vec2{0,1}, glm::vec4(1,1,1,1) };
	Vertex2D br = { glm::vec3{_rW,_rH,0} , glm::vec2{1,1}, glm::vec4(1,1,1,1) };

	vertices.push_back(tl);
	vertices.push_back(tr);
	vertices.push_back(bl);
	vertices.push_back(bl);
	vertices.push_back(tr);
	vertices.push_back(br);

	glBindBuffer(GL_ARRAY_BUFFER, s_vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Camera2D::UpdateFramebuffer()
{
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
				if (draws[k].textureId == draw.textureId && draws[k].shaderId == draw.shaderId &&
					draws[k].clipRect.x == draw.clipRect.x && draws[k].clipRect.y == draw.clipRect.y
					&& draws[k].clipRect.z == draw.clipRect.z && draws[k].clipRect.w == draw.clipRect.w)
				{
					for (int l = 0; l < draw.vertices.size(); l++)
						draws[k].vertices.push_back(draw.vertices[l]);
					break;
				}
			}
			draws.push_back(draw);
		}
	}

	for (int i = 0; i < dbgDraws.size(); i++)
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

	glViewport(0, 0, _w, _h);


	for (int i = 0; i < draws.size(); i++)
	{
		Draw2D draw = draws[i];
		if (draw.shaderId == NULL)
			s->Bind();
		else
		{
			draw.shaderId->Bind();
		}

		if (draw.textureId != NULL)
			draw.textureId->Bind();
		else
			def->Bind();

		if (draw.clipRect.x != -1)
		{
			glEnable(GL_SCISSOR_TEST);
			glScissor(draw.clipRect.x, _h - draw.clipRect.w - draw.clipRect.y, draw.clipRect.z, draw.clipRect.w);
		}
		else
			glDisable(GL_SCISSOR_TEST);

		glBindVertexArray(vao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, draw.vertices.size() * sizeof(Vertex2D), draw.vertices.data(), GL_STATIC_DRAW);

		glDrawArrays(GL_TRIANGLES, 0, draw.vertices.size());

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glBindVertexArray(0);

		if (draw.textureId != NULL)
			draw.textureId->Unbind();
		else
			def->Unbind();

		if (draw.shaderId != NULL)
			draw.shaderId->Unbind();
		else
			s->Unbind();
	}

	glDisable(GL_SCISSOR_TEST);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Camera2D::MouseClick(int button, glm::vec2 pos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		GameObject2D* object = objects[i];
		object->MouseClick(button, pos);
	}
}

void Camera2D::MouseRelease(int button, glm::vec2 pos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		GameObject2D* object = objects[i];
		object->MouseRelease(button, pos);
	}
}

void Camera2D::MouseMove(float x, float y)
{
	for (int i = 0; i < objects.size(); i++)
	{
		GameObject2D* object = objects[i];
		object->MouseMove(x, y);
	}
}

void Camera2D::KeyPress(int key)
{
	for (int i = 0; i < objects.size(); i++)
	{
		GameObject2D* object = objects[i];
		object->KeyPress(key);
	}

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

	static float lT = 0;
	static int framesDone = 0;
	static float fps = 0;

	float ct = glfwGetTime();

	if (ct - lT >= 1.0f)
	{
		fps = framesDone / (ct - lT);
		lT = ct;
		framesDone = 0;
	}
	else
		fps = framesDone / (ct - lT);

	framesDone++;

	std::string format = StringTools::ToTheDecimial(fps, 0);

	DrawDebugText("FPS: " + format, glm::vec2(4, _h - 28), 24);

	UpdateFramebuffer();

	glViewport(0, 0, _rW, _rH);

	DrawSprite();
}

void Camera2D::DrawSprite()
{
	s->Bind();

	if (t != NULL)
		t->Bind();

	glm::mat4 projection = glm::ortho(0.0f, _w, _h, 0.0f);
	s->SetUniformMat4f("projection", &projection[0][0]);

	glBindVertexArray(s_vao);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);

	if (t != NULL)
		t->Unbind();

	s->Unbind();

	glEnable(GL_DEPTH_TEST);

	dbgDraws.clear();
}

void Camera2D::Destroy()
{
	glDeleteFramebuffers(1, &fb);
	glDeleteBuffers(1, &vbo);
	glDeleteVertexArrays(1, &vao);

	glDeleteBuffers(1, &s_vbo);
	glDeleteVertexArrays(1, &s_vao);

	delete s;

	if (t != NULL)
		delete t;

	delete debugText;

	for (int i = 0; i < objects.size(); i++)
	{
		GameObject* object = objects[i];
		object->Destroy();

		delete object;
	}

	objects.clear();

}