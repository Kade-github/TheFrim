#include "Gameplay.h"
#include <Objects/3DText.h>
#include <Objects/3DSprite.h>
#include <Game.h>
#include <Helpers/StringTools.h>
#include "../LightingManager.h"
#include "../MusicManager.h"
#include "../Data/Settings.h"
#include "../CraftingManager.h"
#include "../Objects/Base/Zombie.h"

glm::vec3 rotate(const glm::vec3& v, const glm::vec3& k, float theta)
{
	float cos_theta = cos(theta);
	float sin_theta = sin(theta);

	glm::vec3 rotated = (v * cos_theta) + (glm::cross(k, v) * sin_theta) + (k * glm::dot(k, v)) * (1 - cos_theta);

	return rotated;
}

Gameplay::Gameplay(WorldManager* _wm)
{
	wm = _wm;
}

void Gameplay::Create()
{

	Game::instance->log->Write("Loaded World: " + wm->name);

	Game::instance->log->Write("Loaded " + std::to_string(CraftingManager::GetInstance()->Recipes.size()) + " crafting recipes");

	Game::instance->SetLockedCursor(true);

	Hud::endSequence = false;

	dim = new DroppedItemManager();

	mm = new MobManager();

	c2d = new Camera2D(glm::vec3(0, 0, 0));

	c2d->s = new Shader();

	c2d->s->LoadShader("Assets/Shaders/vert2d.glsl", "Assets/Shaders/frag2d.glsl");

	c2d->order = 3;

	credits = new Camera2D(glm::vec3(0, 0, 0));

	credits->s = c2d->s;

	credits->order = 1000;

	Camera* camera = Game::instance->GetCamera();

	camera->fov = Settings::instance->fov;
	camera->cameraFar = 100.0f * Settings::instance->renderDistance;

	player = new Player(wm->GetPlayerPosition());

	player->order = 2;

	player->playerData = wm->_world.p;

	hud = new Hud(glm::vec3(0, 0, 0), player, c2d);

	hud->hand->order = 2;

	hud->order = 1000;


	wm->SaveWorldNow();

	UpdateChunks();

	celestialSun = new Sprite3D("Assets/Textures/sun.png", glm::vec3(0, 0, 0));
	celestialSun->width = 1;
	celestialSun->height = 1;

	celestialSun->position = glm::vec3(0, 0, 0);
	celestialSun->rotateAxis = glm::vec3(1, 0, 0);

	celestialSun->depth = false;

	celestialMoon = new Sprite3D("Assets/Textures/moon.png", glm::vec3(0, 0, 0));
	celestialMoon->width = 1;
	celestialMoon->height = 1;

	celestialMoon->position = glm::vec3(0, 0, 0);
	celestialMoon->rotateAxis = glm::vec3(1, 0, 0);

	celestialMoon->depth = false;

	celestialStars = new Sprite3D("Assets/Textures/star_bg.png", glm::vec3(0, 0, 0));

	celestialStars->width = 128;
	celestialStars->height = 128;

	celestialStars->UpdateSprite();

	celestialStars->position = glm::vec3(0, 0, 0);
	celestialStars->rotateAxis = glm::vec3(1, 0, 0);

	celestialStars->depth = false;

	glm::vec2 size = Game::instance->GetWindowSize();

	camera->width = size.x;
	camera->height = size.y;

	MusicManager::GetInstance()->GenerateTrackList(); // generate track list

	MusicManager::GetInstance()->nextTrack = glfwGetTime() + 45.0f; // start at 45 seconds

	LightingManager::GetInstance()->sun.angle = wm->_world.sunAngle;
}

void Gameplay::Draw()
{
	LightingManager::GetInstance()->SunUpdate();

	// CHUNK UPDATES
	float currentTime = glfwGetTime();
	bool shouldUpdate = false;
	if (std::abs(currentTime - lastUpdate) >= 0.05f) // 20 times a second
	{

		UpdateChunks();
		shouldUpdate = true;

		lastUpdate = currentTime;

		if (ticks % 10 == 0)
			mm->Update();

		if (!hud->playEnd && Hud::endSequence && !MusicManager::GetInstance()->IsPlaying())
		{
			MusicManager::GetInstance()->PlayMusic("theend", 0.5f);
			hud->playEnd = true;
		}
	}

	if (lastSecond < currentTime)
	{
		tps = ticks - lastTickSecond;
		lastTickSecond = ticks;
		lastSecond = currentTime + 1.0f;

		tickTimes.push_back(tps);

		if (tickTimes.size() > 10)
			tickTimes.erase(tickTimes.begin());
	}

	LightingManager::GetInstance()->SunColor();

	wm->_world.sunAngle = LightingManager::GetInstance()->sun.angle;

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


	celestialSun->position = camera->position - rotate(glm::vec3(0, 0, 8), glm::vec3(1, 0, 0), glm::radians(LightingManager::GetInstance()->sun.angle));
	celestialSun->angle = LightingManager::GetInstance()->sun.angle;

	celestialMoon->position = camera->position + rotate(glm::vec3(0, 0, 8), glm::vec3(1, 0, 0), glm::radians(LightingManager::GetInstance()->sun.angle));
	celestialMoon->angle = LightingManager::GetInstance()->sun.angle;

	Game::instance->shader->Bind();

	glm::mat4 project = camera->GetProjectionMatrix();

	Game::instance->shader->SetUniformMat4f("projection", glm::value_ptr(project));

	int fog = (camera->cameraFar / 2) * Settings::instance->fogDistance;

	if (Settings::instance->fogDistance >= 2.0)
		fog = 10000;

	Game::instance->shader->SetUniform3f("CameraPos", camera->position.x, camera->position.y, camera->position.z);
	Game::instance->shader->SetUniform3f("FogColor", LightingManager::GetInstance()->sun.color.x, LightingManager::GetInstance()->sun.color.y, LightingManager::GetInstance()->sun.color.z);
	Game::instance->shader->SetUniform1f("FogFar", fog);
	Game::instance->shader->SetUniform1f("lightLevel", 10.0f);
	Game::instance->shader->SetUniform1f("redness", 0.0f);
	Game::instance->shader->SetUniform1i("useAmbientDiffusion", (int)Settings::instance->useAmbientDiffuse);

	Game::instance->shader->Unbind();

	if (player->position.y <= -100 && wm->regions.size() != 0)
	{
		Chunk* c = wm->GetChunk(player->position.x, player->position.z);

		player->position.y = c->GetHighestBlock(player->position.x, player->position.z);
	}

	if (c2d->debug)
	{
		c2d->DrawDebugText("Position: " + StringTools::ToTheDecimial(player->position.x, 2) + ", " + StringTools::ToTheDecimial(player->position.y, 2) + ", " + StringTools::ToTheDecimial(player->position.z, 2), glm::vec2(0, 0), 24);
		if (recordBlockData)
		{
			c2d->DrawDebugText("Recording block data (" + std::to_string(blockData.size()) + ")", glm::vec2(0, 24), 24);
		}
		c2d->DrawDebugText("Chunks (Drawn/Loaded): " + std::to_string(chunksRendered) + "/" + std::to_string(chunksLoaded), glm::vec2(0, 48), 24);
	}

	Chunk* currentChunk = wm->GetChunk(player->position.x, player->position.z);

	MusicManager::GetInstance()->Set3DPosition(player->position, camera->cameraFront, camera->cameraUp);

	MusicManager::GetInstance()->Update();

	if (camera->position.y > 170 || LightingManager::GetInstance()->sun.angle > 200) // draw stars skybox
	{
		// celestial stars look at player

		glm::vec3 f = glm::vec3(0, -16, celestialStars->width / 2);

		celestialStars->position = camera->position - f;
		celestialStars->angle = 90;

		celestialStars->Draw();
	}

	celestialSun->Draw();

	celestialMoon->Draw();

	// Draw chunks (regular)
	for (Chunk* c : allChunks)
	{
		if (c->isRendered)
		{
			c->DrawRegular();
			c->DrawModels();
		}
	}

	// Draw chunks (shadow)

	for (Chunk* c : allChunks)
	{
		if (c->isRendered)
			c->DrawShadows();
	}


	// Draw chunks (transparent)

	for (Chunk* c : allChunks)
	{
		if (c->isRendered)
			c->DrawTransparent();
	}

	player->Draw();
	Scene::Draw();

	if (showHud && !Hud::endSequence)
	{
		hud->Draw();

		c2d->Draw();
	}

	if (Hud::endSequence)
		credits->Draw();

	if (shouldUpdate && !hud->GamePaused)
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
			if (!c->isShadowLoaded)
			{
				c->pleaseRender = true;
			}
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

	static std::vector<glm::vec2> toLoadedRegion = {};

	regionsLoaded = 0;

	glm::vec3 fakePos = glm::vec3(player->position.x, 0, player->position.z);

	float regionSize = (CHUNK_SIZE * REGION_SIZE);

	int regionsSize = 0;

	wm->CheckGeneratedRegions();

	wm->GetOrLoadRegion(player->position.x, player->position.z); // always load the current region


	if (!hud->GamePaused)
		ticks++;

	regionsSize = wm->regions.size();

	if (ticks % 2 == 0)
	{
		for (int i = 0; i < regionsSize; i++)
		{
			Region& r = wm->regions[i];
			if (std::find(toLoadedRegion.begin(), toLoadedRegion.end(), glm::vec2(r.startX / regionSize, r.startZ / regionSize)) != toLoadedRegion.end())
			{
				r.loaded = false;
				toLoadedRegion.erase(std::remove(toLoadedRegion.begin(), toLoadedRegion.end(), glm::vec2(r.startX / regionSize, r.startZ / regionSize)), toLoadedRegion.end());
			}

			float distanceToCenter = glm::distance(fakePos, glm::vec3(r.startX + (regionSize / 2), 0, r.startZ + (regionSize / 2)));

			if (distanceToCenter > camera->cameraFar * 3.0f && r.loaded)
			{
				wm->SaveRegion(r.startX, r.startZ);

				// unload
				for (Chunk* c : r.chunks)
				{
					if (c->isLoaded)
					{
						UnloadChunk(c);
						c->myData = {};
						delete c;
					}

					allChunks.erase(std::remove(allChunks.begin(), allChunks.end(), c), allChunks.end());
				}

				r.chunks.clear();
				r.shouldUnload = true;
				r.loaded = false;
				Game::instance->log->Write("Unloading region: " + std::to_string(r.startX) + ", " + std::to_string(r.startZ));

				break;
			}

			if (r.shouldUnload)
			{
				wm->regions.erase(std::remove(wm->regions.begin(), wm->regions.end(), r), wm->regions.end());
				break;
			}

			if (!r.loaded)
			{
				allChunks.insert(allChunks.end(), r.chunks.begin(), r.chunks.end());
			}

			r.loaded = true;
			regionsLoaded++;

			float distanceToLeft = glm::distance(fakePos, glm::vec3(r.startX - regionSize, 0, r.startZ));
			float distanceToRight = glm::distance(fakePos, glm::vec3(r.startX + regionSize, 0, r.startZ));
			float distanceToTop = glm::distance(fakePos, glm::vec3(r.startX, 0, r.startZ - regionSize));
			float distanceToBottom = glm::distance(fakePos, glm::vec3(r.startX, 0, r.startZ + regionSize));

			if (distanceToLeft < camera->cameraFar * 2)
			{
				glm::vec2 pos = glm::vec2((r.startX / regionSize) - 1, r.startZ / regionSize);

				if (!wm->isRegionLoaded(pos.x * regionSize, pos.y * regionSize))
					if (std::find(toLoadedRegion.begin(), toLoadedRegion.end(), pos) == toLoadedRegion.end())
					{
						Game::instance->log->Write("Loading region: " + std::to_string(pos.x * regionSize) + ", " + std::to_string(pos.y * regionSize));
						toLoadedRegion.push_back(pos);
						loadPool.detach_task([this, pos]()
							{
								wm->LoadRegion(pos.x, pos.y);
							});

						// Queue load for edge chunks

						for (Chunk* c : r.chunks)
						{
							if (c->position.x == r.startX)
								UnloadChunk(c);
						}
					}
			}

			if (distanceToRight < camera->cameraFar * 2)
			{
				glm::vec2 pos = glm::vec2((r.startX / regionSize) + 1, r.startZ / regionSize);

				if (!wm->isRegionLoaded(pos.x * regionSize, pos.y * regionSize))
					if (std::find(toLoadedRegion.begin(), toLoadedRegion.end(), pos) == toLoadedRegion.end())
					{
						Game::instance->log->Write("Loading region: " + std::to_string(pos.x * regionSize) + ", " + std::to_string(pos.y * regionSize));
						toLoadedRegion.push_back(pos);
						loadPool.detach_task([this, pos]()
							{
								wm->LoadRegion(pos.x, pos.y);
							});


						// Queue load for edge chunks

						for (Chunk* c : r.chunks)
						{
							if (c->position.x == r.startX + regionSize - 16)
								UnloadChunk(c);
						}
					}
			}

			if (distanceToTop < camera->cameraFar * 2)
			{
				glm::vec2 pos = glm::vec2(r.startX / regionSize, (r.startZ / regionSize) - 1);

				if (!wm->isRegionLoaded(pos.x * regionSize, pos.y * regionSize))
					if (std::find(toLoadedRegion.begin(), toLoadedRegion.end(), pos) == toLoadedRegion.end())
					{
						Game::instance->log->Write("Loading region: " + std::to_string(pos.x * regionSize) + ", " + std::to_string(pos.y * regionSize));
						toLoadedRegion.push_back(pos);
						loadPool.detach_task([this, pos]()
							{
								wm->LoadRegion(pos.x, pos.y);
							});

						// Queue load for edge chunks

						for (Chunk* c : r.chunks)
						{
							if (c->position.z == r.startZ)
								UnloadChunk(c);
						}
					}
			}

			if (distanceToBottom < camera->cameraFar * 2)
			{
				glm::vec2 pos = glm::vec2(r.startX / regionSize, (r.startZ / regionSize) + 1);

				if (!wm->isRegionLoaded(pos.x * regionSize, pos.y * regionSize))
					if (std::find(toLoadedRegion.begin(), toLoadedRegion.end(), pos) == toLoadedRegion.end())
					{
						Game::instance->log->Write("Loading region: " + std::to_string(pos.x * regionSize) + ", " + std::to_string(pos.y * regionSize));
						toLoadedRegion.push_back(pos);
						loadPool.detach_task([this, pos]()
							{
								wm->LoadRegion(pos.x, pos.y);
							});

						// Queue load for edge chunks

						for (Chunk* c : r.chunks)
						{
							if (c->position.z == r.startZ + regionSize - 16)
								UnloadChunk(c);
						}
					}
			}
		}


		// sort chunks by distance

		std::sort(allChunks.begin(), allChunks.end(), [camera](Chunk* a, Chunk* b)
			{
				glm::vec3 fakePosA = glm::vec3(a->position.x, camera->position.y, a->position.z);
				glm::vec3 fakePosB = glm::vec3(b->position.x, camera->position.y, b->position.z);

				float distanceA = glm::distance(camera->position, fakePosA);
				float distanceB = glm::distance(camera->position, fakePosB);

				return distanceA < distanceB;
			});
	}

	chunksLoaded = 0;
	chunksRendered = 0;

	int fog = (camera->cameraFar / 2) * Settings::instance->fogDistance;

	float fov = Settings::instance->fov;

	if (Settings::instance->fogDistance >= 2.0)
		fog = 10000;

	for (Chunk* c : allChunks)
	{
		glm::vec3 fakePosC = glm::vec3(c->position.x + 8, 0, c->position.z + 8);

		float distance = glm::distance(fakePos, fakePosC);

		if (distance < camera->cameraFar)
		{
			if (c->id < 0)
			{
				c->id = 1;
				c->Init();
			}

			if (!c->isLoaded)
			{
				QueueLoad(c);
				return;
			}

			float angle = glm::degrees(glm::acos(glm::dot(glm::normalize(fakePos - fakePosC), glm::normalize(camera->cameraFront))));

			if ((angle >= 25 || distance <= 32) && distance <= fog + 64)
			{
				if (!c->isRendered || c->pleaseRender)
				{
					c->pleaseRender = false;
					c->SetBuffer();
					c->SetTransparentBuffer();
					c->SetShadowBuffer();
				}
				c->isRendered = true;
			}
			else
			{
				if (c->isRendered)
					c->isRendered = false;
			}
		}
		else
		{
			if (c->isLoaded)
			{
				UnloadChunk(c);
				break;
			}
		}

		// Chunk updates

		if (c->isLoaded && !hud->GamePaused && !c->isBeingLoaded)
		{
			chunksLoaded++;
			c->UpdateChunk(ticks);
		}

		if (c->isRendered)
		{
			chunksRendered++;
		}
	}

}

void Gameplay::UnloadChunk(Chunk* c)
{
	c->Unload();
	c->isLoaded = false;
	c->isRendered = false;
	c->isShadowLoaded = false;
	c->isBeingLoaded = false;
	c->pleaseRender = false;
}


void Gameplay::KeyPress(int key)
{
	if (key == GLFW_KEY_F5)
	{
		Chunk* c = wm->GetChunk(player->position.x, player->position.z);

		if (c != nullptr)
			UnloadChunk(c);

		hud->ShowHint("Reloading chunk");
	}

	if (key == GLFW_KEY_Y)
	{
		Data::InventoryItem item(Data::ITEM_ROCKET, 1);

		player->playerData.GiveItem(item);

		hud->UpdateHotbar();

		hud->ShowHint("Gave you a rocket");
	}

	if (key == GLFW_KEY_F7)
	{
		player->freeCam = !player->freeCam;

		hud->ShowHint("Free cam: " + std::to_string(player->freeCam));
	}

	if (key == GLFW_KEY_F10)
	{
		LightingManager::GetInstance()->sun.angle += 15;

		hud->ShowHint("Sun angle: " + std::to_string(LightingManager::GetInstance()->sun.angle));
	}


	/*if (key == GLFW_KEY_P)
	{
		recordBlockData = !recordBlockData;
	}

	if (key == GLFW_KEY_R)
	{
		recordBlockData = !recordBlockData;

		if (!recordBlockData)
		{
			std::string data = "// Generated in game\nBlueprint b;\n";

			int negXWidth = 0;
			int xWidth = 0;
			int negZWidth = 0;
			int zWidth = 0;

			for (auto& [v, value] : blockData)
			{
				std::vector<std::string> spl = StringTools::Split(v, ",");
				glm::vec3 key = glm::vec3(std::stoi(spl[0]), std::stoi(spl[1]), std::stoi(spl[2]));

				if (key.x < negXWidth)
					negXWidth = key.x;

				if (key.x > xWidth)
					xWidth = key.x;

				if (key.z < negZWidth)
					negZWidth = key.z;

				if (key.z > zWidth)
					zWidth = key.z;

				data += "b.blocks.push_back(BlueprintBlock((BlockType)" + std::to_string(value) + ", " + std::to_string(key.x) + ", " + std::to_string(key.y) + ", " + std::to_string(key.z) + "));\n";
			}

			data += "b.negXWidth = " + std::to_string(negXWidth) + ";\n";
			data += "b.xWidth = " + std::to_string(xWidth) + ";\n";
			data += "b.negZWidth = " + std::to_string(negZWidth) + ";\n";
			data += "b.zWidth = " + std::to_string(zWidth) + ";\n";

			data += "variations.push_back(b);";

			// write to file

			std::ofstream file("recorded_blueprint.cpp");

			file << data;

			file.close();
		}
		else
			firstBlock = glm::vec3(0, 0, 0);

		blockData.clear();
	}*/

	if (key == GLFW_KEY_F12)
	{
		showHud = !showHud;
	}

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->KeyPress(key);
	}

	c2d->KeyPress(key);

	hud->KeyPress(key);
	player->KeyPress(key);
}

void Gameplay::MouseClick(int button, glm::vec2 mPos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->MouseClick(button, mPos);
	}

	hud->MouseClick(button, mPos);
	player->MouseClick(button, mPos);
}

void Gameplay::MouseRelease(int button, glm::vec2 mPos)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->MouseRelease(button, mPos);
	}

	hud->MouseRelease(button, mPos);
	player->MouseRelease(button, mPos);
}

void Gameplay::OnScroll(double x, double y)
{
	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->OnScroll(x, y);
	}

	hud->OnScroll(x, y);
	player->OnScroll(x, y);
}

void Gameplay::FocusChange(bool focus)
{
	static bool wasPaused = false;

	if (hud->inv->shown)
		return;

	if (!focus)
	{
		wasPaused = Hud::GamePaused;
		if (!wasPaused)
			player->TogglePauseMenu();
	}
	else if (focus && !wasPaused)
	{
		player->TogglePauseMenu();
	}

}

void Gameplay::Destroy()
{
	wm->_world.p = player->playerData;

	wm->SetPlayerPosition(player->position);
	wm->SaveWorldNow();

	for (Region& r : wm->regions)
	{
		for (Chunk* c : r.chunks)
		{
			UnloadChunk(c);
			c->myData = {};
			delete c;
		}

		r.chunks.clear();
	}

	allChunks.clear();

	wm->regions.clear();

	delete wm;

	dim->RemoveItems();

	delete dim;

	delete player;
	delete hud;

	c2d->Destroy();

	delete c2d;

	delete celestialSun;
	delete celestialMoon;
	delete celestialStars;
}

void Gameplay::Resize(float _w, float _h)
{
	if (c2d != nullptr)
		c2d->Resize();
}
