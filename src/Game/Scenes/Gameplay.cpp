#include "Gameplay.h"
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

	Camera* camera = Game::instance->GetCamera();

	player = new Player(glm::vec3(0, 128, 0));

	AddObject(player);

	UpdateChunks();

	Chunk* c = wm->GetChunk(player->position.x, player->position.z);

	if (c != NULL)
	{
		Block* b = c->GetTopBlock(player->position.x, player->position.z);

		int y = b->position.y;
		player->position.y = y + 3;
	}

	Game::instance->CaptureCursor(true);
}

void Gameplay::Draw()
{
	Camera* camera = Game::instance->GetCamera();

	c2d->DrawDebugText("Player Position: " + StringTools::ToTheDecimial(player->position.x, 2) + ", " + StringTools::ToTheDecimial(player->position.y, 2) + ", " + StringTools::ToTheDecimial(player->position.z, 2), glm::vec2(4, 32), 24);

	c2d->DrawDebugText("Camera Far: " + StringTools::ToTheDecimial(camera->cameraFar, 2), glm::vec2(4, 64), 24);

	UpdateChunks();

	Scene::Draw();
}

void Gameplay::UpdateChunks()
{
	Camera* camera = Game::instance->GetCamera();

	for (Region& r : wm->regions)
	{
		for (Chunk* c : r.chunks)
		{
			c->data = r.GetChunkData(c->position.x, c->position.z);

			if (c->id < 0)
			{
				AddObject(c);
			}

			float diff = glm::distance(glm::vec3(c->position.x,128, c->position.z), glm::vec3(camera->position.x, 128, camera->position.z));

			if (diff < camera->cameraFar)
			{
				if (!c->isLoaded)
				{
					// set neighbors

					c->frontChunk = wm->GetChunk(c->position.x, c->position.z + 16);
					if (c->frontChunk != NULL)
					{
						c->frontChunk->data = r.GetChunkData(c->frontChunk->position.x, c->frontChunk->position.z);
					}
					c->backChunk = wm->GetChunk(c->position.x, c->position.z - 16);
					if (c->backChunk != NULL)
					{
						c->backChunk->data = r.GetChunkData(c->backChunk->position.x, c->backChunk->position.z);
					}
					c->leftChunk = wm->GetChunk(c->position.x - 16, c->position.z);
					if (c->leftChunk != NULL)
					{
						c->leftChunk->data = r.GetChunkData(c->leftChunk->position.x, c->leftChunk->position.z);
					}
					c->rightChunk = wm->GetChunk(c->position.x + 16, c->position.z);
					if (c->rightChunk != NULL)
					{
						c->rightChunk->data = r.GetChunkData(c->rightChunk->position.x, c->rightChunk->position.z);
					}

					c->UpdateBlocks(r.GetChunkData(c->position.x, c->position.z));
				}

				float angle = camera->YawAngleTo(c->position);

				if (angle <= 200 || diff < (camera->cameraFar / 2))
				{
					if (!c->rendered)
						c->GenerateMesh();
				}
				else
				{
					if (c->rendered)
						c->DeloadMesh();
				}
			}
			else
			{
				if (c->isLoaded)
					c->DeloadBlocks();
			}
		}
	}
}

void Gameplay::KeyPress(int key)
{
	if (key == GLFW_KEY_F2)
	{
		for (Region& r : wm->regions)
		{
			for (Chunk* c : r.chunks)
			{
				c->DeloadMesh();
				c->DeloadBlocks();
			}
		}

	}

	if (key == GLFW_KEY_F3)
		wm->SaveWorldNow();


	if (key == GLFW_KEY_F5)
		player->position.y = player->position.y + 10;

	if (key == GLFW_KEY_F6)
	{
		Camera* c = Game::instance->GetCamera();
		c->cameraFar += 25;
	}

	if (key == GLFW_KEY_F7)
	{
		Camera* c = Game::instance->GetCamera();
		c->cameraFar -= 25;
	}

	if (key == GLFW_KEY_F8)
	{
		Region& r = wm->GetRegion(player->position.x, player->position.z);

		Chunk* c = wm->GetChunk(player->position.x, player->position.z);
		c->UpdateBlocks(r.GetChunkData(c->position.x, c->position.z));
	}

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
