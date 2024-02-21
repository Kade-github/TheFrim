#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include "../zstr/src/zstr.hpp"

#include <filesystem>

WorldManager* WorldManager::instance = nullptr;

std::mutex mtx;
std::mutex chunk_mtx;

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
				LoadChunks();
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			} });
	_generateThread.detach();
}


void WorldManager::LoadRegion(int x, int z)
{
	Data::Region r = _world.getRegion(x, z);

	if (r.startX == 0 && r.startZ == 0 && r.endX == 0 && r.endZ == 0)
	{
		GenerateRegion(x, z);
		return;
	}
	else
	{
		std::vector<Chunk*> chunks = CreateChunksInRegion(r);

		regions.push_back({ r.startX, r.startZ, r.endX, r.endZ, r, chunks });
	}
}

void WorldManager::UnloadRegion(Region& r)
{
	for (auto& _c : r.chunks)
	{
		if (_c->isLoaded)
			_c->UnloadMesh();
		if (_c->rendered)
			_c->Clean();
		delete _c;
	}

	{
		std::lock_guard<std::mutex> lock(mtx);
		regions.erase(std::remove(regions.begin(), regions.end(), r), regions.end());
	}
}

std::vector<Chunk*> WorldManager::CreateChunksInRegion(Data::Region& r)
{
	std::vector<Chunk*> chunks;

	for (auto& c : r.chunks)
	{
		Chunk* chunk = new Chunk(glm::vec3(c.x, 0, c.z), instance->texturePack);
		chunks.push_back(chunk);
		chunkGenerated(chunk);
	}

	return chunks;

}

void WorldManager::LoadChunks()
{
	Camera* camera = Game::instance->GetCamera();

	std::vector<glm::vec2> toLoad;

	{
		std::lock_guard<std::mutex> lock(mtx);
		for (int i = 0; i < regions.size(); i++)
		{
			Region& r = regions[i];
			int index = 0;
			for (auto& c : r.chunks)
			{
				glm::vec3 re = glm::vec3(c->position.x, 128, c->position.z);
				glm::vec3 cam = glm::vec3(camera->position.x, 128, camera->position.z);

				float diff = glm::distance(cam, re);

				if (diff <= camera->cameraFar)
				{
					if (!c->isLoaded)
					{
						Data::Chunk data = r.data.getChunk(c->position.x, c->position.z);
						Data::Chunk forward = r.data.getChunk(c->position.x, c->position.z + 16);
						Data::Chunk backward = r.data.getChunk(c->position.x, c->position.z - 16);
						Data::Chunk left = r.data.getChunk(c->position.x - 16, c->position.z);
						Data::Chunk right = r.data.getChunk(c->position.x + 16, c->position.z);



						c->GenerateMesh(data, forward, backward, left, right);
					}
				}
				else
				{
					if (c->isLoaded)
					{
						c->UnloadMesh();
					}
				}
				index++;
			}
		}
	}

	for (auto& c : toLoad)
	{
		LoadRegion(c[0], c[1]);
	}
}

void WorldManager::GenerateRegion(int x, int z)
{
	Data::Region r = _world.generateRegion(x * (128), z * (128));

	std::vector<Chunk*> chunks = CreateChunksInRegion(r);
	{
		std::lock_guard<std::mutex> lock(mtx);
		regions.push_back({ r.startX, r.startZ, r.endX, r.endZ, r, chunks });
	}
}


void WorldManager::CreateWorld()
{
	_world = Data::World();

	_world.name = "Test";

	_world._path = std::filesystem::current_path().string() + "/worlds/" + _world.name;

	// Generate regions

	// generate 4x4 regions

	for (int x = -2; x < 2; x++)
	{
		for (int z = -2; z < 2; z++)
		{
			GenerateRegion(x, z);
		}
	}
	
	SaveWorld();
}

void WorldManager::LoadWorld()
{
	zstr::ifstream is_p(_path + "/world.frim", std::ios::binary);

	std::stringstream buffer;

	buffer << is_p.rdbuf();

	msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());

	is_p.close();

	upd.get().convert(_world);

	// save directroy

	_world._path = std::filesystem::current_path().string() + "/worlds/" + _world.name;

	// load regions

	LoadRegion(0, 0);

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

			for (auto& r : regions)
			{
				_world.saveRegion(r.data);
			}

			Game::instance->log->Write("World saved to " + _path); 
		})
		.detach();
}

void WorldManager::RenderChunks()
{
	Camera* camera = Game::instance->GetCamera();

	renderedChunks = 0;

	for (int i = 0; i < regions.size(); i++)
	{
		Region& r = regions[i];
		for (auto& c : r.chunks)
		{
			if (!c->isLoaded)
				continue;

			glm::vec3 re = glm::vec3(c->position.x, 128, c->position.z);
			glm::vec3 cam = glm::vec3(camera->position.x, 128, camera->position.z);

			float diff = glm::distance(cam, re);
			float angle = camera->YawAngleTo(c->position);

			if (diff <= 50 || angle <= 200)
			{
				if (!c->rendered)
				{
					c->UploadMesh();
					renderedChunks++;
				}
			}
			else if (c->rendered)
			{
				c->Clean();
			}
			
		}
	}
}
