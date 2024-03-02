#include "Gameplay.h"
#include <Game.h>
#include <imgui.h>


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

	c2d->DrawDebugText("Player Position: " + std::to_string((int)player->position.x) + ", " + std::to_string((int)player->position.y) + ", " + std::to_string((int)player->position.z), glm::vec2(4,32), 24);

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
						c->frontChunk->backChunk = c;
					}
					c->backChunk = wm->GetChunk(c->position.x, c->position.z - 16);
					if (c->backChunk != NULL)
					{
						c->backChunk->data = r.GetChunkData(c->backChunk->position.x, c->backChunk->position.z);
						c->backChunk->frontChunk = c;
					}
					c->leftChunk = wm->GetChunk(c->position.x - 16, c->position.z);
					if (c->leftChunk != NULL)
					{
						c->leftChunk->data = r.GetChunkData(c->leftChunk->position.x, c->leftChunk->position.z);
						c->leftChunk->rightChunk = c;
					}
					c->rightChunk = wm->GetChunk(c->position.x + 16, c->position.z);
					if (c->rightChunk != NULL)
					{
						c->rightChunk->data = r.GetChunkData(c->rightChunk->position.x, c->rightChunk->position.z);
						c->rightChunk->leftChunk = c;
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
	if (key == GLFW_KEY_F3)
		wm->SaveWorldNow();

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
