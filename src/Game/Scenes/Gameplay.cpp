#include "Gameplay.h"
#include <Objects/3DText.h>
#include <Objects/3DSprite.h>
#include <Game.h>
#include <Helpers/StringTools.h>
#include "../LightingManager.h"
#include "../MusicManager.h"
#include "../Data/Settings.h"
#include "../CraftingManager.h"

Gameplay::Gameplay(WorldManager* _wm)
{
	wm = _wm;
}

void Gameplay::Create()
{
	CraftingManager::GetInstance()->Init();

	Game::instance->log->Write("Loaded World: " + wm->name);

	Game::instance->log->Write("Loaded " + std::to_string(CraftingManager::GetInstance()->Recipes.size()) + " crafting recipes");

	Game::instance->SetLockedCursor(true);

	dim = new DroppedItemManager();

	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	AddObject(c2d);

	c2d->order = 3;

	Camera* camera = Game::instance->GetCamera();

	camera->fov = Settings::instance->fov;
	camera->cameraFar = 100.0f * Settings::instance->renderDistance;

	player = new Player(wm->GetPlayerPosition());

	AddObject(player);

	player->order = 2;

	hud = new Hud(glm::vec3(0, 0, 0), player, c2d);

	AddObject(hud);

	hud->order = 1000;

	UpdateChunks();

	loadPool.reset(std::thread::hardware_concurrency() * 3.14f);

	MusicManager::GetInstance()->GenerateTrackList(); // generate track list

	MusicManager::GetInstance()->nextTrack = 45.0f; // start at 45 seconds

	LightingManager::GetInstance()->sun.angle = 90; // set to noon
}

void Gameplay::Draw()
{
	LightingManager::GetInstance()->SunUpdate();
	LightingManager::GetInstance()->SunColor();

	if (LightingManager::GetInstance()->sun.angle >= 180) // night time
	{
		if (!MusicManager::GetInstance()->ambient)
		{
			MusicManager::GetInstance()->ambient = true;
			MusicManager::GetInstance()->GenerateAmbientTrackList();
		}
	}
	else
	{
		if (MusicManager::GetInstance()->ambient)
		{
			MusicManager::GetInstance()->ambient = false;
			MusicManager::GetInstance()->GenerateTrackList();
		}
	}

	Camera* camera = Game::instance->GetCamera();

	Game::instance->shader->Bind();

	int fog = (camera->cameraFar / 2) * Settings::instance->fogDistance;

	if (Settings::instance->fogDistance >= 1.19)
		fog = 10000;

	Game::instance->shader->SetUniform3f("CameraPos", camera->position.x, camera->position.y, camera->position.z);
	Game::instance->shader->SetUniform3f("FogColor", LightingManager::GetInstance()->sun.color.x, LightingManager::GetInstance()->sun.color.y, LightingManager::GetInstance()->sun.color.z);
	Game::instance->shader->SetUniform1f("FogFar", fog);

	c2d->DrawDebugText("Player Position: " + StringTools::ToTheDecimial(player->position.x, 2) + ", " + StringTools::ToTheDecimial(player->position.y, 2) + ", " + StringTools::ToTheDecimial(player->position.z, 2), glm::vec2(4, 4), 24);

	c2d->DrawDebugText("Sun: " + StringTools::ToTheDecimial(LightingManager::GetInstance()->sun.angle, 2) + ", Progress: " + StringTools::ToTheDecimial(LightingManager::GetInstance()->sun.angle / 360, 2), glm::vec2(4, 34), 24);

	c2d->DrawDebugText("Camera Far: " + StringTools::ToTheDecimial(camera->cameraFar, 2), glm::vec2(4, 64), 24);

	c2d->DrawDebugText("Regions: " + std::to_string(wm->regions.size()), glm::vec2(4, 94), 24);

	c2d->DrawDebugText("Music Manager: " + std::to_string(glfwGetTime()) + "/" + std::to_string(MusicManager::GetInstance()->nextTrack), glm::vec2(4, 124), 24);


	UpdateChunks();

	MusicManager::GetInstance()->Update();

	Scene::Draw();

	dim->Update(); // these use delayed 

	if (camera->vertices.size() != 0)
		camera->DebugDraws();
}

void Gameplay::QueueLoad(Chunk* c)
{
	if (c->isBeingLoaded)
		return;

	c->isBeingLoaded = true;
	c->isLoaded = false;

	loadPool.detach_task([c]()
		{
			c->CreateSubChunks();

			c->RenderSubChunks();
			c->RenderSubChunksShadow();
			c->pleaseRender = true;
			c->isLoaded = true;
			c->isBeingLoaded = false;
		});
}

void Gameplay::QueueLoadBlocks(Chunk* c)
{
	if (c->isBeingLoaded)
		return;

	c->isBeingLoaded = true;

	loadPool.detach_task([c]()
		{
			c->CreateSubChunks();
			c->RenderSubChunks();
			c->pleaseRender = true;
			c->isBeingLoaded = false;
		});
}

void Gameplay::QueueShadow(Chunk* c)
{
	if (c->isShadowLoaded)
		return;

	c->isShadowLoaded = true;

	loadPool.detach_task([c]()
		{
			while (c->isBeingLoaded)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
			c->RenderSubChunksShadow();
			c->pleaseRender = true;
			c->isShadowLoaded = false;
		});
}

void Gameplay::UpdateChunks()
{
	Camera* camera = Game::instance->GetCamera();

	wm->CheckGeneratedRegions();

	static std::vector<glm::vec2> toLoadedRegion = {};

	for (Region& r : wm->regions)
	{
		// if the startx and startz is equal to a toLoadedRegion value, then remove it from the vector

		for (int i = 0; i < toLoadedRegion.size(); i++)
		{
			if (r.startX == toLoadedRegion[i].x && r.startZ == toLoadedRegion[i].y)
			{
				toLoadedRegion.erase(toLoadedRegion.begin() + i);
				break;
			}
		}

		for (Chunk* c : r.chunks)
		{
			if (c->id < 0)
			{
				c->Init();
				AddObject(c);
			}

			glm::vec3 fakePos = glm::vec3(c->position.x, camera->position.y, c->position.z);

			float distance = glm::distance(camera->position, fakePos);

			if (distance < camera->cameraFar * 1.5)
			{
				if (!c->isLoaded)
				{
					QueueLoad(c);
					return;
				}

				float angle = camera->YawAngleTo(fakePos);

				if (angle < 200 || distance <= 32)
				{
					if (!c->isRendered || c->pleaseRender)
					{
						c->pleaseRender = false;
						c->SetBuffer();
						c->SetShadowBuffer();
					}
					c->isRendered = true;
				}
				else
				{
					if (!c->isRendered)
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

		int amount = (CHUNK_SIZE * REGION_SIZE);

		glm::vec3 fakePosR = glm::vec3(r.startX + amount, camera->position.y, r.startZ);

		float distanceR = glm::distance(camera->position, glm::vec3(fakePosR.x, fakePosR.y, camera->position.z));

		if (distanceR < camera->cameraFar * 1.5 && !wm->isRegionLoaded(fakePosR.x, fakePosR.z))
		{
			bool no = false;

			for (int i = 0; i < toLoadedRegion.size(); i++)
			{
				if (fakePosR.x == toLoadedRegion[i].x && fakePosR.z == toLoadedRegion[i].y)
				{
					no = true;
					break;
				}
			}
			if (!no)
			{
				toLoadedRegion.push_back(glm::vec2(fakePosR.x, fakePosR.z));
				wm->_generatePool.detach_task([this, r, amount]()
					{
						wm->GenerateRegion((r.startX + amount) / amount, r.startZ / amount);
					});
			}
		}
		else if (distanceR > camera->cameraFar * 1.5 && wm->isRegionLoaded(fakePosR.x, fakePosR.z))
		{
			// deload

			Game::instance->log->Write("Deloaded region: " + std::to_string(fakePosR.x) + ", " + std::to_string(fakePosR.z));

			for (int i = 0; i < wm->regions.size(); i++)
			{
				if (wm->regions[i].startX == fakePosR.x && wm->regions[i].startZ == fakePosR.z)
				{
					for (Chunk* c : wm->regions[i].chunks)
					{
						if (c->isLoaded)
						{
							c->Unload();
							c->isLoaded = false;
						}
					}
					wm->SaveRegion(fakePosR.x, fakePosR.z);
					wm->regions.erase(wm->regions.begin() + i);
					break;
				}
			}
			break;
		}

		fakePosR = glm::vec3(r.startX, camera->position.y, r.startZ + amount);

		distanceR = glm::distance(camera->position, glm::vec3(camera->position.x, fakePosR.y, fakePosR.z));

		if (distanceR < camera->cameraFar * 1.5 && !wm->isRegionLoaded(fakePosR.x, fakePosR.z))
		{
			bool no = false;

			for (int i = 0; i < toLoadedRegion.size(); i++)
			{
				if (fakePosR.x == toLoadedRegion[i].x && fakePosR.z == toLoadedRegion[i].y)
				{
					no = true;
					break;
				}
			}
			if (!no)
			{
				toLoadedRegion.push_back(glm::vec2(fakePosR.x, fakePosR.z));
				wm->_generatePool.detach_task([this, r, amount]()
					{
						wm->GenerateRegion(r.startX / amount, (r.startZ + amount) / amount);
					});
			}
		}
		else if (distanceR > camera->cameraFar * 1.5 && wm->isRegionLoaded(fakePosR.x, fakePosR.z))
		{
			// deload

			Game::instance->log->Write("Deloaded region: " + std::to_string(fakePosR.x) + ", " + std::to_string(fakePosR.z));

			for (int i = 0; i < wm->regions.size(); i++)
			{
				if (wm->regions[i].startX == fakePosR.x && wm->regions[i].startZ == fakePosR.z)
				{
					for (Chunk* c : wm->regions[i].chunks)
					{
						if (c->isLoaded)
						{
							c->Unload();
							c->isLoaded = false;
						}
					}
					wm->SaveRegion(fakePosR.x, fakePosR.z);
					wm->regions.erase(wm->regions.begin() + i);
					break;
				}
			}
			break;
		}

		fakePosR = glm::vec3(r.startX, camera->position.y, r.startZ);

		distanceR = glm::distance(camera->position, glm::vec3(fakePosR.x, fakePosR.y, camera->position.z));

		if (distanceR < camera->cameraFar * 1.5 && !wm->isRegionLoaded(fakePosR.x - amount, fakePosR.z))
		{
			bool no = false;

			for (int i = 0; i < toLoadedRegion.size(); i++)
			{
				if (fakePosR.x - amount == toLoadedRegion[i].x && fakePosR.z == toLoadedRegion[i].y)
				{
					no = true;
					break;
				}
			}
			if (!no)
			{
				toLoadedRegion.push_back(glm::vec2(fakePosR.x - amount, fakePosR.z));
				wm->_generatePool.detach_task([this, r, amount]()
					{
						wm->GenerateRegion((r.startX - amount) / amount, r.startZ / amount);
					});
			}
		}
		else if (distanceR > camera->cameraFar * 1.5 && wm->isRegionLoaded(fakePosR.x - amount, fakePosR.z))
		{
			// deload

			Game::instance->log->Write("Deloaded region: " + std::to_string(fakePosR.x) + ", " + std::to_string(fakePosR.z));

			for (int i = 0; i < wm->regions.size(); i++)
			{
				if (wm->regions[i].startX == fakePosR.x - amount && wm->regions[i].startZ == fakePosR.z)
				{
					for (Chunk* c : wm->regions[i].chunks)
					{
						if (c->isLoaded)
						{
							c->Unload();
							c->isLoaded = false;
						}
					}
					wm->SaveRegion(fakePosR.x - amount, fakePosR.z);
					wm->regions.erase(wm->regions.begin() + i);
					break;
				}
			}
			break;
		}

		fakePosR = glm::vec3(r.startX, camera->position.y, r.startZ);

		distanceR = glm::distance(camera->position, glm::vec3(camera->position.x, fakePosR.y, fakePosR.z));

		if (distanceR < camera->cameraFar * 1.5 && !wm->isRegionLoaded(fakePosR.x, fakePosR.z - amount))
		{
			bool no = false;

			for (int i = 0; i < toLoadedRegion.size(); i++)
			{
				if (fakePosR.x == toLoadedRegion[i].x && fakePosR.z - amount == toLoadedRegion[i].y)
				{
					no = true;
					break;
				}
			}
			if (!no)
			{
				toLoadedRegion.push_back(glm::vec2(fakePosR.x, fakePosR.z - amount));
				wm->_generatePool.detach_task([this, r, amount]()
					{
						wm->GenerateRegion(r.startX / amount, (r.startZ - amount) / amount);
					});
			}
		}
		else if (distanceR > camera->cameraFar * 1.5 && wm->isRegionLoaded(fakePosR.x, fakePosR.z - amount))
		{
			// deload

			Game::instance->log->Write("Deloaded region: " + std::to_string(fakePosR.x) + ", " + std::to_string(fakePosR.z));

			for (int i = 0; i < wm->regions.size(); i++)
			{
				if (wm->regions[i].startX == fakePosR.x && wm->regions[i].startZ == fakePosR.z - amount)
				{
					for (Chunk* c : wm->regions[i].chunks)
					{
						if (c->isLoaded)
						{
							c->Unload();
							c->isLoaded = false;
						}
						RemoveObject(c);
					}
					wm->SaveRegion(fakePosR.x, fakePosR.z - amount);

					wm->regions.erase(wm->regions.begin() + i);
					break;
				}
			}
			break;
		}

	}
}

void Gameplay::KeyPress(int key)
{

	if (key == GLFW_KEY_F3)
		MusicManager::GetInstance()->PlayNext();

	if (key == GLFW_KEY_F5)
		wm->SaveWorldNow();

	if (key == GLFW_KEY_F6)
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


	if (key == GLFW_KEY_F7)
		player->freeCam = !player->freeCam;

	if (key == GLFW_KEY_F10)
		LightingManager::GetInstance()->sun.angle += 25;

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

void Gameplay::MouseRelease(int button, glm::vec2 mPos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->MouseRelease(button, mPos);
	}
}

void Gameplay::OnScroll(double x, double y)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->OnScroll(x, y);
	}
}

void Gameplay::FocusChange(bool focus)
{
	static bool wasPaused = false;
	if (!focus)
	{
		wasPaused = Hud::GamePaused;
		
		player->TogglePauseMenu();
	}
	else if (focus && !wasPaused)
	{
		player->TogglePauseMenu();
	}
}

void Gameplay::Destroy()
{
	wm->SetPlayerPosition(player->position);

	wm->SaveWorldNow();
	delete wm;
	delete dim;
}

void Gameplay::Resize(float _w, float _h)
{
	if (c2d != nullptr)
		c2d->Resize();
}
