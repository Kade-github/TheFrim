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

	_generatePool.reset(5);

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
		int realX = x / 80;
		int realZ = z / 80;
		GenerateRegion(realX, realZ);
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
	Game::instance->log->Write("Unloading region " + std::to_string(r.startX) + "," + std::to_string(r.startZ));

	instance->_world.saveRegion(r.data);

	for (auto& _c : r.chunks)
	{
		if (_c->isLoaded)
			_c->UnloadMesh();
		if (_c->rendered)
			_c->Clean();
		Game::instance->currentScene->RemoveObject(_c);
		delete _c;
	}

	regions.erase(std::remove(regions.begin(), regions.end(), r), regions.end());
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

Data::Region WorldManager::GetRegion(int _x, int _z)
{
	int x = _x / 80;
	int z = _z / 80;
	// if the region is already loaded, lets not waste time.
	for (auto& r : regions)
	{
		if (r.startX == x && r.startZ == z && r.endX == x + 80 && r.endZ == z + 80)
			return r.data;
	}

	return _world.getRegion(x, z, x + 80, z + 80);
}

std::vector<Chunk*> WorldManager::CreateChunksInRegion(Data::Region& r)
{
	std::vector<Chunk*> chunks;

	for (int x = 0; x < 5; x++)
		for (int z = 0; z < 5; z++)
		{
			Chunk* chunk = new Chunk(glm::vec3(r.startX + (x * 16), 0, r.startZ + (z * 16)), instance->texturePack);
			chunks.push_back(chunk);
		}

	return chunks;
}

void WorldManager::LoadChunks()
{
	Camera* camera = Game::instance->GetCamera();
	std::vector<glm::vec4> toGen;
	glm::vec3 cPos = glm::vec3(camera->position.x, 128, camera->position.z);

	{
		std::lock_guard<std::mutex> lock(mtx);
		for (int i = 0; i < regions.size(); i++)
		{
			Region& r = regions[i];

			if (!r.loaded)
				continue;

			for (auto& c : r.chunks)
			{

				glm::vec3 re = glm::vec3(c->position.x, 128, c->position.z);

				float diff = glm::distance(cPos, re);

				if (diff <= camera->cameraFar)
				{
					if (!c->isLoaded)
					{
						_generatePool.detach_task([c, i]()
							{
								Data::Chunk data = instance->regions[i].data.getChunk(c->position.x, c->position.z);
								Data::Chunk forward = instance->regions[i].data.getChunk(c->position.x, c->position.z + 16);
								Data::Chunk backward = instance->regions[i].data.getChunk(c->position.x, c->position.z - 16);
								Data::Chunk left = instance->regions[i].data.getChunk(c->position.x - 16, c->position.z);
								Data::Chunk right = instance->regions[i].data.getChunk(c->position.x + 16, c->position.z);

								c->GenerateMesh(data, forward, backward, left, right);
							});
					}
				}
				else
				{
					if (c->isLoaded)
					{
						c->UnloadMesh();
					}
				}

				int realX = r.startX / 80;
				int realZ = r.startZ / 80;

				glm::vec3 checkPos = glm::vec3(r.startX - 1, 128, r.startZ);
				float dist = glm::distance(cPos, checkPos);

				if (dist <= camera->cameraFar)
					instance->ShouldLoad(realX - 1, realZ);

				checkPos = glm::vec3(r.startX, 128, r.startZ - 1);
				dist = glm::distance(cPos, checkPos);

				if (dist <= camera->cameraFar)
					instance->ShouldLoad(realX, realZ - 1);

				checkPos = glm::vec3(r.startX + 1, 128, r.startZ);
				dist = glm::distance(cPos, checkPos);

				if (dist <= camera->cameraFar)
					instance->ShouldLoad(realX + 1, realZ);

				checkPos = glm::vec3(r.startX, 128, r.startZ + 1);
				dist = glm::distance(cPos, checkPos);

				if (dist <= camera->cameraFar)
					instance->ShouldLoad(realX, realZ + 1);
			}
		}

		_generatePool.wait();
	}
}

void WorldManager::ShouldLoad(int x, int z)
{
	int realX = x * 80;
	int realZ = z * 80;

	if (!IsRegionLoaded(realX, realZ, realX + 80, realZ + 80))
		LoadRegion(realX, realZ, realX + 80, realZ + 80);
}

void WorldManager::GenerateRegion(int x, int z)
{
	int _x = x * 80;
	int _z = z * 80;

	Game::instance->log->Write("Generating region " + std::to_string(_x) + " " + std::to_string(_z));

	Data::Region r = _world.generateRegion(_x, _z);

	std::vector<Chunk*> chunks = CreateChunksInRegion(r);
	regions.push_back({ _x, _z, _x + 80, _z + 80, r, chunks });
}

void WorldManager::CreateWorld()
{
	_world = Data::World();

	_world.name = "test";

	_world._path = std::filesystem::current_path().string() + "/worlds/" + _world.name;

	// Generate regions

	{
		std::lock_guard<std::mutex> lock(mtx);
		GenerateRegion(0, 0);
		GenerateRegion(-1, 0);
		GenerateRegion(1, 0);
		GenerateRegion(0, 1);
		GenerateRegion(0, -1);
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

	LoadRegion(0, 0, 80, 80);
}

void WorldManager::SaveWorld()
{
	std::thread([&]()
		{
			Game::instance->log->Write("Saving world...");

			if (!std::filesystem::exists(_path))
				std::filesystem::create_directories(_path);

			zstr::ofstream os_p(_path + "/world.frim", std::ios::binary);

			msgpack::pack(os_p, _world);

			for (auto& r : regions)
			{
				_world.saveRegion(r.data);
			}

			Game::instance->log->Write("World saved to " + _path); })
		.detach();
}

void WorldManager::RenderChunks()
{
	Camera* camera = Game::instance->GetCamera();
	glm::vec3 cPos = glm::vec3(camera->position.x, 128, camera->position.z);
	renderedChunks = 0;

	for (int i = 0; i < regions.size(); i++)
	{
		Region& r = regions[i];

		if (!r.loaded)
			continue;

		glm::vec3 thisPos = glm::vec3(r.startX, 128, r.startZ);
		float thisDistance = glm::distance(cPos, thisPos);

		if (thisDistance > camera->cameraFar * 3)
		{
			r.loaded = false;
			std::lock_guard<std::mutex> lock(mtx);
			UnloadRegion(r);
			break;
		}

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
