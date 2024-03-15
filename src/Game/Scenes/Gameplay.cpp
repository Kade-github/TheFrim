#include "Gameplay.h"
#include <Objects/3DText.h>
#include <Game.h>
#include <imgui.h>
#include <Helpers/StringTools.h>

Gameplay::Gameplay(WorldManager* _wm)
{
	wm = _wm;
}

void Gameplay::Create()
{
	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	c2d->order = 1;

	crosshair = new Sprite2D("Assets/Textures/crosshair.png", glm::vec3(c2d->_w / 2, c2d->_h / 2, 0));

	c2d->AddObject(crosshair);
	crosshair->order = 1;

	Camera* camera = Game::instance->GetCamera();

	player = new Player(glm::vec3(0, 128, 0));

	AddObject(player);

	UpdateChunks();

	Game::instance->CaptureCursor(true);
}

void Gameplay::Draw()
{
	Camera* camera = Game::instance->GetCamera();

	crosshair->position = glm::vec3((c2d->_w / 2) - crosshair->width / 2, (c2d->_h / 2) - crosshair->height / 2, 0);

	Game::instance->shader->Bind();
	Game::instance->shader->SetUniform3f("lightPos", player->position.x, player->position.y, player->position.z);

	c2d->DrawDebugText("Player Position: " + StringTools::ToTheDecimial(player->position.x, 2) + ", " + StringTools::ToTheDecimial(player->position.y, 2) +  ", " + StringTools::ToTheDecimial(player->position.z, 2), glm::vec2(4, 4), 24);

	c2d->DrawDebugText("Camera Far: " + StringTools::ToTheDecimial(camera->cameraFar, 2), glm::vec2(4, 64), 24);

	UpdateChunks();

	Scene::Draw();

	if (camera->vertices.size() != 0)
		camera->DebugDraws();
}

void Gameplay::UpdateChunks()
{
	Camera* camera = Game::instance->GetCamera();

	for (Region& r : wm->regions)
	{
		for (Chunk* c : r.chunks)
		{
			if (c->id < 0)
				AddObject(c);

			float distance = glm::distance(camera->position, c->position);

			if (distance < 512)
			{
				if (!c->IsLoaded())
				{
					c->Init(); // when destroyed you gotta reinit
					c->CreateSubChunks();
				}
				else
				{
					if (!c->isRendered())
					{
						c->RenderSubChunks();
						c->SetBuffer();
					}
				}
			}
			else
			{
				if (c->IsLoaded())
				{
					c->Unload();
				}
			}
		}
	}
}

void Gameplay::KeyPress(int key)
{

	if (key == GLFW_KEY_F3)
		wm->SaveWorldNow();

	if (key == GLFW_KEY_F2)
	{
		for (Region& r : wm->regions)
		{
			for (Chunk* c : r.chunks)
			{
				if (c->IsLoaded())
					c->Unload();
			}
		}
	}

	if (key == GLFW_KEY_F6)
		player->freeCam = !player->freeCam;

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->KeyPress(key);
	}
}

void Gameplay::MouseClick(int button, glm::vec2 mPos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->MouseClick(button, mPos);
	}
}

void Gameplay::Destroy()
{
	wm->SaveWorldNow();
	delete wm;
}
