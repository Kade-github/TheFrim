#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include "../zstr/src/zstr.hpp"

#include <filesystem>

WorldManager* WorldManager::instance = nullptr;

std::mutex mtx;
std::mutex chunk_mtx;

bool WorldManager::IsRegionLoaded(int x, int z, int endX, int endZ)
{
	for (auto& r : regions)
	{
		if (r.startX == x && r.startZ == z && r.endX == endX && r.endZ == endZ)
			return true;
	}

	return false;
}

WorldManager::WorldManager(std::string worldPath, Texture* _tp)
{
	instance = this;

	_path = worldPath;
	texturePack = _tp;

	if (!std::filesystem::exists(_path))
		CreateWorld();
	else
		LoadWorld();

	_generateThread = std::thread([&]()
		{
			while (true)
			{
				LoadChunks();
				std::this_thread::sleep_for(std::chrono::milliseconds(10));
			} });
	_generateThread.detach();
}


void WorldManager::LoadRegion(int x, int z, int endX, int endZ)
{
	Data::Region r = GetRegion(x, z, endX, endZ);

	if (r.startX == 0 && r.startZ == 0 && r.endX == 0 && r.endZ == 0)
	{
		GenerateRegion(x / 128, z / 128);
		return;
	}
	else
	{
		Game::instance->log->Write("Loading region " + std::to_string(r.startX) + " " + std::to_string(r.startZ) + " " + std::to_string(r.endX) + " " + std::to_string(r.endZ));
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

Data::Region WorldManager::GetRegion(int x, int z, int endX, int endZ)
{
	// if the region is already loaded, lets not waste time.
	for (auto& r : regions)
	{
		if (r.startX == x && r.startZ == z && r.endX == endX && r.endZ == endZ)
			return r.data;
	}

	return _world.getRegion(x, z, endX, endZ);
}

std::vector<Chunk*> WorldManager::CreateChunksInRegion(Data::Region& r)
{
	std::vector<Chunk*> chunks;

	for (auto& c : r.chunks)
	{
		Chunk* chunk = new Chunk(glm::vec3(c.x, 0, c.z), instance->texturePack);
		chunks.push_back(chunk);
	}

	return chunks;

}

void WorldManager::LoadChunks()
{
	Camera* camera = Game::instance->GetCamera();
	std::vector<glm::vec4> toGen;

	{
		std::lock_guard<std::mutex> lock(mtx);
		for (int i = 0; i < regions.size(); i++)
		{
			Region& r = regions[i];

			int loadedChunks = 0;

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
						Data::Chunk data = regions[i].data.getChunk(c->position.x, c->position.z);
						Data::Chunk forward = regions[i].data.getChunk(c->position.x, c->position.z + 16);
						Data::Chunk backward = regions[i].data.getChunk(c->position.x, c->position.z - 16);
						Data::Chunk left = regions[i].data.getChunk(c->position.x - 16, c->position.z);
						Data::Chunk right = regions[i].data.getChunk(c->position.x + 16, c->position.z);

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

				if (c->isLoaded)
					loadedChunks++;

				index++;
			}

			// check in each direction if we need to load more regions

			glm::vec3 r1 = glm::vec3(r.startX, 128, r.startZ - 16);
			glm::vec3 r2 = glm::vec3(r.startX - 16, 128, r.startZ);
			glm::vec3 r3 = glm::vec3(r.endX, 128, r.endZ + 16);
			glm::vec3 r4 = glm::vec3(r.endX + 16, 128, r.endZ);

			glm::vec3 cam = glm::vec3(camera->position.x, 128, camera->position.z);

			float diff1 = glm::distance(cam, r1);
			float diff2 = glm::distance(cam, r2);
			float diff3 = glm::distance(cam, r3);
			float diff4 = glm::distance(cam, r4);

			if (diff1 <= camera->cameraFar)
			{
				if (!IsRegionLoaded(r.endX, r.endZ, r.endX + 128, r.endZ + 128))
					toGen.push_back(glm::vec4(r.endX, r.endZ, r.endX + 128, r.endZ + 128));
			}

			if (diff2 <= camera->cameraFar)
			{
				if (!IsRegionLoaded(r.startX - 128, r.startZ - 128, r.startX, r.startZ))
					toGen.push_back(glm::vec4((r.startX - 128), (r.startZ - 128), r.startX, r.startZ));
			}

			if (diff3 <= camera->cameraFar)
			{
				if (!IsRegionLoaded(r.startX - 128, r.endZ, r.startX, r.endZ + 128))
					toGen.push_back(glm::vec4(r.startX - 128, r.endZ, r.startX, r.endZ + 128));
	
			}

			if (diff4 <= camera->cameraFar)
			{
				if (!IsRegionLoaded(r.endX, r.startZ - 128, r.endX + 128, r.startZ))
					toGen.push_back(glm::vec4(r.endX, r.startZ - 128, r.endX + 128, r.startZ));
			}
		}

		for (auto& r : toGen)
		{
			LoadRegion(r.x, r.y, r.z, r.w);
		}
	}
}

void WorldManager::GenerateRegion(int x, int z)
{
	int _x = x * 128;
	int _z = z * 128;

	Game::instance->log->Write("Generating region " + std::to_string(_x) + " " + std::to_string(_z));

	Data::Region r = _world.generateRegion(_x, _z);


	std::vector<Chunk*> chunks = CreateChunksInRegion(r);
	regions.push_back({ r.startX, r.startZ, r.endX, r.endZ, r, chunks });
}


void WorldManager::CreateWorld()
{
	_world = Data::World();

	_world.name = "Test";

	_world._path = std::filesystem::current_path().string() + "/worlds/" + _world.name;

	// Generate regions

	// generate 4x4 regions

	{
		std::lock_guard<std::mutex> lock(mtx);
		for (int x = -2; x < 2; x++)
		{
			for (int z = -2; z < 2; z++)
			{
				GenerateRegion(x, z);
			}
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

	_world.scanForRegions();

	Game::instance->log->Write("Loading world " + _world.name + " from " + _path + ". Found: " + std::to_string(_world.storedRegions.size()) + " regions.");

	LoadRegion(0, 0, 128, 128);

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
			if (c->id < 0)
			{
				Game::instance->currentScene->AddObject(c);
			}

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
