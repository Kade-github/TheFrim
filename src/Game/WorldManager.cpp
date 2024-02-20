#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include "../zstr/src/zstr.hpp"

#include <filesystem>

WorldManager *WorldManager::instance = nullptr;

std::mutex mtx;

void WorldManager::CreateChunk(Chunk* pC)
{
	if (pC->forward == NULL)
		pC->forward = GetChunk(pC->position.x, pC->position.z + 16);
	if (pC->backward == NULL)
		pC->backward = GetChunk(pC->position.x, pC->position.z - 16);
	if (pC->left == NULL)
		pC->left = GetChunk(pC->position.x - 16, pC->position.z);
	if (pC->right == NULL)
		pC->right = GetChunk(pC->position.x + 16, pC->position.z);

	chunkGenerated(pC);
	chunks.push_back(pC);
}

WorldManager::WorldManager(std::string worldPath, Texture *_tp, std::function<void(Chunk *)> onGenerated)
{
	instance = this;

	_path = worldPath;
	texturePack = _tp;
	chunkGenerated = onGenerated;

	if (!std::filesystem::exists(_path))
		CreateWorld();
	else
		LoadWorld();

	_edgeThread = std::thread([&]()
							  {
			while (true)
			{
				instance->GenerateEdgeChunks();
				std::this_thread::sleep_for(std::chrono::milliseconds(400));
			} });

	_generateThread = std::thread([&]()
								  {
			while (true)
			{
				GenerateMeshes();
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			} });
	_generateThread.detach();

	_edgeThread.detach();
}

Chunk *WorldManager::GetLoadedChunk(int x, int z)
{
	for (auto &c : loadedChunks)
	{
		if (c->position.x == x && c->position.z == z)
			return c;
	}

	return nullptr;
}

void WorldManager::UploadChunks()
{
	// figure out which chunks should be loaded

	loadedChunks = {};

	for (auto &c : chunks)
	{

		if (c->isLoaded && !c->rendered)
		{
			c->UploadMesh();
			loadedChunks.push_back(c);
		}
		else if (!c->isLoaded && c->rendered)
			c->UnloadMesh();
	}
}

Data::Chunk *WorldManager::GenerateChunk(int x, int z)
{
	Data::World *w = &_world;

	Game::instance->log->Write("Generating chunk at " + std::to_string(x) + " " + std::to_string(z));

	Data::Chunk c = w->generateChunk(x, z);

	c.isGenerated = true;

	Chunk *ch = new Chunk(glm::vec3(c.x, c.y, c.z), texturePack);

	ch->forward = GetChunk(ch->position.x, ch->position.z + 16);
	ch->backward = GetChunk(ch->position.x, ch->position.z - 16);
	ch->left = GetChunk(ch->position.x - 16, ch->position.z);
	ch->right = GetChunk(ch->position.x + 16, ch->position.z);

	Data::Chunk* d = nullptr;

	{
		std::lock_guard<std::mutex> lock(mtx);
		w->chunks.push_back(c);
		d = &w->chunks.back();
		CreateChunk(ch);
	}

	return d;
}

void WorldManager::CreateWorld()
{

	using namespace Data;

	_world = World();

	_world.name = "Test";

	// generate 4x4 chunks

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	World *w = &_world;

	for (int x = -10; x < 10; x++)
	{
		for (int z = -10; z < 10; z++)
		{
			_generatePool.detach_task([x, z]()
									  { instance->GenerateChunk(x * 16, z * 16); });
		}
	}

	SaveWorld();
}

void WorldManager::LoadWorld()
{
	zstr::ifstream is_p(_path, std::ios::binary);

	std::stringstream buffer;

	buffer << is_p.rdbuf();

	msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());

	is_p.close();

	upd.get().convert(_world);

	std::cout << "Loaded " + std::to_string(_world.chunks.size()) + " chunks" << std::endl;

	for (auto &c : _world.chunks)
	{
		c.isGenerated = true;
		Chunk *ch = new Chunk(glm::vec3(c.x, c.y, c.z), texturePack);
		_generatePool.detach_task([ch]()
								  {
									  {
										  std::lock_guard<std::mutex> lock(mtx);
										  instance->CreateChunk(ch);
									  } });
	}
}

void WorldManager::SaveWorld()
{
	std::thread([&]()
				{
			Game::instance->log->Write("Saving world...");
			_generatePool.wait();

			zstr::ofstream os_p(_path, std::ios::binary);

			msgpack::pack(os_p, _world);

			Game::instance->log->Write("World saved to " + _path); })
		.detach();
}

void WorldManager::GenerateMeshes()
{
	Camera *camera = Game::instance->GetCamera();

	for (int i = 0; i < chunks.size(); i++)
	{
		Chunk *c = chunks[i];
		glm::vec3 r = c->position;
		r.y = 128;

		float diff = std::abs(glm::distance(r, camera->position));

		float angle = atan2(camera->position.z - r.z, camera->position.x - r.x);

		if (diff < camera->cameraFar && angle > -180 && angle < 180)
		{
			if (!c->isLoaded)
				c->GenerateMesh(_world.chunks[i]);
		}
		else if (c->isLoaded)
			c->UnloadMesh();
	}
}

void WorldManager::GenerateEdgeChunks()
{
	Camera *camera = Game::instance->GetCamera();

	for (auto &c : loadedChunks)
	{
		if (c->forward == NULL)
		{
			glm::vec3 forwardChunk = glm::vec3(c->position.x, 128, c->position.z + 16);
			float diff = std::abs(glm::distance(forwardChunk, camera->position));

			float angle = atan2(camera->position.z - forwardChunk.z, camera->position.x - forwardChunk.x);

			if (diff < camera->cameraFar && angle < 180 && angle > -180)
				c->forward = GenerateChunk(c->position.x, c->position.z + 16);
		}

		if (c->backward == NULL)
		{
			glm::vec3 backwardChunk = glm::vec3(c->position.x, 128, c->position.z - 16);
			float diff = std::abs(glm::distance(backwardChunk, camera->position));

			float angle = atan2(camera->position.z - backwardChunk.z, camera->position.x - backwardChunk.x);

			if (diff < camera->cameraFar && angle < 180 && angle > -180)
				c->backward = GenerateChunk(c->position.x, c->position.z - 16);
		}

		if (c->left == NULL)
		{
			glm::vec3 leftChunk = glm::vec3(c->position.x - 16, 128, c->position.z);
			float diff = std::abs(glm::distance(leftChunk, camera->position));

			float angle = atan2(camera->position.z - leftChunk.z, camera->position.x - leftChunk.x);

			if (diff < camera->cameraFar && angle < 180 && angle > -180)
				c->left = GenerateChunk(c->position.x - 16, c->position.z);
		}

		if (c->right == NULL)
		{
			glm::vec3 rightChunk = glm::vec3(c->position.x + 16, 128, c->position.z);
			float diff = std::abs(glm::distance(rightChunk, camera->position));

			float angle = atan2(camera->position.z - rightChunk.z, camera->position.x - rightChunk.x);

			if (diff < camera->cameraFar && angle < 180 && angle > -180)
				c->right = GenerateChunk(c->position.x + 16, c->position.z);
		}
	}
}

Data::Chunk *WorldManager::GetChunk(int x, int z)
{
	for (auto &c : _world.chunks)
	{
		if (c.isGenerated)
			if (c.x == x && c.z == z)
				return &c;
	}

	return nullptr;
}
