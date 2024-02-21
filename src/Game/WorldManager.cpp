#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include "../zstr/src/zstr.hpp"

#include <filesystem>

WorldManager* WorldManager::instance = nullptr;

std::mutex mtx;

void WorldManager::CreateChunk(Chunk* pC)
{
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

	_generateThread = std::thread([&]()
		{
			while (true)
			{
				GenerateMeshes();
				std::this_thread::sleep_for(std::chrono::milliseconds(250));
			} });
	_generateThread.detach();
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

Chunk* WorldManager::GenerateChunk(int x, int z)
{
	Chunk* ch = nullptr;
	Game::instance->log->Write("Generating chunk at " + std::to_string(x) + " " + std::to_string(z));
	Data::Chunk c = instance->_world.generateChunk(x, z);

	c.isGenerated = true;

	ch = new Chunk(glm::vec3(c.x, 0, c.z), texturePack);
	
	instance->_world.addChunk(c);

	return ch;
}

void WorldManager::EdgeCheck(Chunk* c)
{
	Data::Chunk forwardC = instance->GetChunk(c->position.x, c->position.z + 16);
	Data::Chunk backwardC = instance->GetChunk(c->position.x, c->position.z - 16);
	Data::Chunk leftC = instance->GetChunk(c->position.x - 16, c->position.z);
	Data::Chunk rightC = instance->GetChunk(c->position.x + 16, c->position.z);

	if (!forwardC.isGenerated)
	{
		Chunk* t = instance->GenerateChunk(c->position.x, c->position.z + 16);
		forwardC = instance->GetChunk(c->position.x, c->position.z + 16);
		instance->CreateChunk(t);
	}

	if (!backwardC.isGenerated)
	{
		Chunk* t = instance->GenerateChunk(c->position.x, c->position.z - 16);
		backwardC = instance->GetChunk(c->position.x, c->position.z - 16);
		instance->CreateChunk(t);
	}

	if (!leftC.isGenerated)
	{
		Chunk* t = instance->GenerateChunk(c->position.x - 16, c->position.z);
		leftC = instance->GetChunk(c->position.x - 16, c->position.z);
		instance->CreateChunk(t);
	}

	if (!rightC.isGenerated)
	{
		Chunk* t = instance->GenerateChunk(c->position.x + 16, c->position.z);
		rightC = instance->GetChunk(c->position.x + 16, c->position.z);
		instance->CreateChunk(t);
	}

	c->GenerateMesh(instance->GetChunk(c->position.x, c->position.z), forwardC, backwardC, leftC, rightC);
}


void WorldManager::CreateWorld()
{

	_world = Data::World();

	_world.name = "Test";



	for (int x = -10; x < 10; x++)
	{
		for (int z = -10; z < 10; z++)
		{
			_generatePool.detach_task([x, z]()
				{
					Chunk* c = instance->GenerateChunk(x * 16, z * 16);
					instance->CreateChunk(c);
				});
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
		Chunk* ch = new Chunk(glm::vec3(c.x, 0, c.z), texturePack);
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

			if (!std::filesystem::exists(_path))
				std::filesystem::create_directories(_path);

			zstr::ofstream os_p(_path + "/world.frim", std::ios::binary);

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
		std::vector<Chunk*> chun = chunks;
		for (int i = 0; i < chun.size(); i++)
		{
			Chunk* c = chun[i];


			glm::vec3 r = c->position;
			r.y = 128;

			float diff = std::abs(glm::distance(r, camera->position));

			if (diff <= camera->cameraFar)
			{
				if (c->isLoaded)
					continue;
				instance->EdgeCheck(c);
			}
			else
				c->UnloadMesh();

		}
		_generatePool.wait();
	}
}


Data::Chunk WorldManager::GetChunk(int x, int z)
{
	return _world.getChunk(x, z);
}
