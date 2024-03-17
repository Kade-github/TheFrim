#include "Gameplay.h"
#include <Objects/3DText.h>
#include <Game.h>
#include <imgui.h>
#include <Helpers/StringTools.h>
#include "../LightingManager.h"

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

	LightingManager::GetInstance()->sun.angle = 90; // set to noon

	Game::instance->CaptureCursor(true);
}

void Gameplay::Draw()
{
	LightingManager::GetInstance()->SunUpdate();
	LightingManager::GetInstance()->SunColor();

	Camera* camera = Game::instance->GetCamera();

	crosshair->position = glm::vec3((c2d->_w / 2) - crosshair->width / 2, (c2d->_h / 2) - crosshair->height / 2, 0);

	c2d->DrawDebugText("Player Position: " + StringTools::ToTheDecimial(player->position.x, 2) + ", " + StringTools::ToTheDecimial(player->position.y, 2) +  ", " + StringTools::ToTheDecimial(player->position.z, 2), glm::vec2(4, 4), 24);

	c2d->DrawDebugText("Sun: " + StringTools::ToTheDecimial(LightingManager::GetInstance()->sun.angle, 2) + ", Progress: " + StringTools::ToTheDecimial(LightingManager::GetInstance()->sun.angle / 360, 2), glm::vec2(4, 34), 24);

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
			{
				c->Init();
				AddObject(c);
			}
			glm::vec3 fakePos = glm::vec3(c->position.x, camera->position.y, c->position.z);

			float distance = glm::distance(camera->position, fakePos);

			if (distance < camera->cameraFar)
			{
				if (!c->isLoaded)
				{
					c->CreateSubChunks();

					c->RenderSubChunks();
					c->SetBuffer();
					c->RenderSubChunksShadow();
					c->SetShadowBuffer();

					c->isLoaded = true;
				}

				float angle = camera->YawAngleTo(fakePos);

				if (angle < 200 || distance < 32)
				{
					c->isRendered = true;
				}
				else
				{
					c->isRendered = false;
				}
			}
			else
			{
				if (c->isLoaded)
				{
					c->Unload();
					c->isLoaded = false;
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
				if (c->isLoaded)
				{
					c->Unload();
					c->isLoaded = false;
				}
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
