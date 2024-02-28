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

	wm->RenderChunks();

	Chunk* c = wm->GetChunk(player->position.x, player->position.z);

	if (c != NULL)
	{
		int y = c->getTopBlock(player->position.x, player->position.z);
		player->position.y = y + 8;
	}


	Game::instance->CaptureCursor(true);
}

void Gameplay::Draw()
{

	Camera* camera = Game::instance->GetCamera();

	wm->RenderChunks();

	Scene::Draw();
}

void Gameplay::KeyPress(int key)
{
	if (key == GLFW_KEY_F2)
		wm->ReloadChunks();
	if (key == GLFW_KEY_F3)
		wm->SaveWorldNow();

	for (int i = 0; i < objects.size(); i++)
	{
		objects[i]->KeyPress(key);
	}
}

void Gameplay::Destroy()
{
	wm->SaveWorldNow();
	delete wm;
}
