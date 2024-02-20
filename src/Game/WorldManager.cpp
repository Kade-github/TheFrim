#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include "../zstr/src/zstr.hpp"

#include <filesystem>

WorldManager* WorldManager::instance = nullptr;

std::mutex mtx;

void WorldManager::CreateChunk(Chunk* pC)
{
	pC->forwardC = GetChunk(pC->position.x, pC->position.z + 16);
	pC->backwardC = GetChunk(pC->position.x, pC->position.z - 16);
	pC->leftC = GetChunk(pC->position.x - 16, pC->position.z);
	pC->rightC = GetChunk(pC->position.x + 16, pC->position.z);

	chunkGenerated(pC);
	chunks.push_back(pC);
}

WorldManager::WorldManager(std::string worldPath, Texture* _tp, std::function<void(Chunk*)> onGenerated)
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

Chunk* WorldManager::GetLoadedChunk(int x, int z)
{
	for (auto& c : loadedChunks)
	{
		if (c->position.x == x && c->position.z == z)
			return c;
	}

	return nullptr;
}

void WorldManager::UploadChunks()
{
	// figure out which chunks should be rendered and loaded

	Camera* camera = Game::instance->GetCamera();

	loadedChunks = {};
	renderedChunks = 0;

	for (int i = 0; i < chunks.size(); i++)
	{
		Chunk* c = chunks[i];

		if (!c->isLoaded)
			continue;

		glm::vec3 r = c->position;
		r.y = 128;

		// angle from yaw

		float angle = camera->YawAngleTo(r);
		float diff = std::abs(glm::distance(r, camera->position));

		if (angle <= 180 || diff <= 40)
		{
			if (!c->rendered)
				c->UploadMesh();
		}
		else if (c->rendered)
			c->Clean();

		if (c->rendered)
			renderedChunks++;

		loadedChunks.push_back(c);
	}
}

void WorldManager::GenerateChunk(int x, int z)
{
	Data::World* w = &_world;

	Game::instance->log->Write("Generating chunk at " + std::to_string(x) + " " + std::to_string(z));

	Data::Chunk c = w->generateChunk(x, z);

	c.isGenerated = true;

	Chunk* ch = new Chunk(glm::vec3(c.x, c.y, c.z), texturePack);

	{
		std::lock_guard<std::mutex> lock(mtx);
		w->chunks.push_back(c);
		CreateChunk(ch);
	}
}

void WorldManager::CreateWorld()
{

	using namespace Data;

	_world = World();

	_world.name = "Test";

	// generate 4x4 chunks

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

	World* w = &_world;

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

	for (auto& c : _world.chunks)
	{
		c.isGenerated = true;
		Chunk* ch = new Chunk(glm::vec3(c.x, c.y, c.z), texturePack);
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
	_generatePool.wait();

	Camera* camera = Game::instance->GetCamera();
	{
		std::lock_guard<std::mutex> lock(mtx);
		for (int i = 0; i < chunks.size(); i++)
		{
			Chunk* c = chunks[i];
			glm::vec3 r = c->position;
			r.y = 128;

			float diff = std::abs(glm::distance(r, camera->position));

			_generatePool.detach_task([c, diff, i, camera]()
				{
					if (diff < camera->cameraFar)
					{
						if (!c->isLoaded)
							c->GenerateMesh(&instance->_world.chunks[i]);
					}
					else if (c->isLoaded)
						c->UnloadMesh();
				});

		}
		_generatePool.wait();
	}
}

void WorldManager::GenerateEdgeChunks()
{
	Camera* camera = Game::instance->GetCamera();

	// TODO
	
}

Data::Chunk* WorldManager::GetChunk(int x, int z)
{
	for (int i = 0; i < _world.chunks.size(); i++)
	{
		Data::Chunk& c = _world.chunks[i];
		if (c.isGenerated)
			if (c.x == x && c.z == z)
				return &c;
	}

	return nullptr;
}
