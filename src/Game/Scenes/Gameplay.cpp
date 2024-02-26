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

	Chunk* c = wm->GetChunk(player->position.x, player->position.z);

	if (c != nullptr)
		player->position.y = c->topBlocks[(int)player->position.x % 16][(int)player->position.z % 16];

	AddObject(player);

	Game::instance->CaptureCursor(true);
}

void Gameplay::Draw()
{
	Camera* camera = Game::instance->GetCamera();

	ImGui::Begin("Debug", 0, ImGuiWindowFlags_AlwaysAutoResize);

	ImGui::Text("Player Pos: %f, %f, %f", player->position.x, player->position.y, player->position.z);

	ImGui::SliderFloat("Render Distance", &camera->cameraFar, 32.0f, 400.0f);

	ImGui::SliderFloat("Player Speed", &player->playerSpeed, 1.0f, 100.0f);

	ImGui::End();

	wm->RenderChunks();

	Scene::Draw();
}

void Gameplay::KeyPress(int key)
{
	if (key == GLFW_KEY_F2)
		wm->ReloadChunks();
	if (key == GLFW_KEY_F3)
		wm->SaveWorldNow();
}

void Gameplay::Destroy()
{
	wm->SaveWorldNow();
	delete wm;
}
