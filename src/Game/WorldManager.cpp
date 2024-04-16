#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include <Helpers/StringTools.h>
#include "../zstr/src/zstr.hpp"

#include <filesystem>

std::mutex generateMutex;

WorldManager* WorldManager::instance = nullptr;

WorldManager::WorldManager(std::string name, Texture* _tp, std::string _seed)
{
	instance = this;

	if (!std::filesystem::exists("worlds"))
		std::filesystem::create_directories("worlds");

	_path = "worlds/" + name;
	texturePack = _tp;

	if (!std::filesystem::exists(_path))
		CreateWorld(_seed, name);
	else
		LoadWorldFile();
}

std::vector<Data::World> WorldManager::GetWorlds()
{
	if (!std::filesystem::exists("worlds"))
		std::filesystem::create_directories("worlds");

	std::vector<Data::World> worlds;

	for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path().string() + "/worlds"))
	{
		std::string path = entry.path().string();
		if (entry.is_directory() && std::filesystem::exists(path + "/world.frim"))
		{
			Data::World _world;

			zstr::ifstream is_p(path + "/world.frim", std::ios::binary);

			std::stringstream buffer;

			buffer << is_p.rdbuf();

			msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());

			is_p.close();

			upd.get().convert(_world);

			_world._path = path;

			worlds.push_back(_world);
		}
	}

	return worlds;
}

WorldManager::WorldManager(Data::World _world, Texture* _tp)
{
	_world = _world;
	texturePack = _tp;
	_generatePool.reset(6);
}

WorldManager::WorldManager(Texture* _tp)
{
	texturePack = _tp;
	_generatePool.reset(6);
}

void WorldManager::CreateWorld(std::string _seed, std::string _name)
{
	_world = Data::World();

	std::string s = _seed;

	if (s.size() == 0) // generate random seed
		s = StringTools::gen_random(20);

	if (s.size() > 20)
		s = s.substr(0, 20);

	_world.name = _name;
	_world.seed = s;

	_world.parseSeed();

	_world._path = std::filesystem::current_path().string() + "/worlds/" + _world.name;

	// Generate regions

	for(int i = -1; i < 2; i++)
		for (int j = -1; j < 2; j++)
		{
			_generatePool.detach_task([i, j]() {
				instance->GenerateRegion(i, j);
			});
		}
}

bool WorldManager::IsRegionGenerated(int x, int z)
{
	bool found = false;

	bool loaded = false;

	for (auto& r : regions)
	{
		if (r.startX == x && r.startZ == z)
		{
			found = true;
			loaded = true;
			break;
		}
	}

	if (found)
		return true;

	if (!loaded)
	{
		Data::Region r = _world.getRegion(x, z);

		if (r.startX == 0 && r.startZ == 0 && r.endX == 0 && r.endZ == 0)
			found = false;
	}

	return found;
}

void WorldManager::CreateChunks(Region& r)
{
	for (int x = r.startX; x < r.endX; x += CHUNK_SIZE)
	{
		for (int z = r.startZ; z < r.endZ; z += CHUNK_SIZE)
		{
			Chunk* c = new Chunk(texturePack, glm::vec3(x, 0, z));
			c->myData = r.GetChunkData(x, z);
			r.chunks.push_back(c);
		}
	}
}

void WorldManager::CheckGeneratedRegions()
{
	if (_generatedRegions.size() != 0)
	{
		std::lock_guard<std::mutex> lock(generateMutex);
		for (Region r : _generatedRegions)
			regions.push_back(r);

		_generatedRegions.clear();
	}
}

void WorldManager::GenerateRegion(int x, int z)
{
	Data::Region r = _world.generateRegion(x, z);

	r.generateStructures();

	Region reg(r, std::vector<Chunk*>());

	CreateChunks(reg);

	std::lock_guard<std::mutex> lock(generateMutex);
	_generatedRegions.push_back(reg);

	Game::instance->log->Write("Generated region " + std::to_string(x) + ", " + std::to_string(z));
}

void WorldManager::LoadWorldFile()
{
	zstr::ifstream is_p(_path + "/world.frim", std::ios::binary);

	std::stringstream buffer;

	buffer << is_p.rdbuf();

	msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());

	is_p.close();

	upd.get().convert(_world);

	// save directroy

	_world._path = std::filesystem::current_path().string() + "/worlds/" + _world.name;
	_world.parseSeed();

	// load regions

	_world.scanForRegions();

	Game::instance->log->Write("Loading world " + _world.name + " from " + _path + ". Found: " + std::to_string(_world.storedRegions.size()) + " regions.");

	LoadWorld();

	
}

void WorldManager::LoadWorld()
{
	int playerX = _world.p.x;
	int playerZ = _world.p.z;

	// initial
	_generatePool.detach_task([&, playerX, playerZ]() {
		int x = playerX / (CHUNK_SIZE * REGION_SIZE);
		int z = playerZ / (CHUNK_SIZE * REGION_SIZE);
		instance->LoadRegion(x,z);
		instance->LoadRegion(x + 1, z);
		instance->LoadRegion(x - 1, z);
		instance->LoadRegion(x, z + 1);
		instance->LoadRegion(x, z - 1);
		instance->LoadRegion(x + 1, z + 1);
		instance->LoadRegion(x - 1, z - 1);
		instance->LoadRegion(x + 1, z - 1);
		instance->LoadRegion(x - 1, z + 1);
	});

}

Region& WorldManager::GetRegion(float x, float z)
{
	for (auto& r : regions)
	{
		if (r.startX <= x && r.startZ <= z && r.endX > x && r.endZ > z)
			return r;
	}

	return regions[0];
}

void WorldManager::LoadRegion(int x, int z)
{
	if (!IsRegionGenerated(x, z))
	{
		GenerateRegion(x, z);
		return;
	}

	Data::Region r = _world.getRegion(x, z);

	Region reg(r, std::vector<Chunk*>());

	CreateChunks(reg);

	std::lock_guard<std::mutex> lock(generateMutex);
	_generatedRegions.push_back(reg);

	Game::instance->log->Write("Loaded region " + std::to_string(x) + ", " + std::to_string(z));
}

void WorldManager::SaveRegion(int x, int z)
{
	_world.saveRegion(GetRegion(x, z).data);
}

bool WorldManager::isRegionLoaded(float x, float z)
{
	for (auto& r : regions)
	{
		if (r.startX <= x && r.startZ <= z && r.endX > x && r.endZ > z)
			return true;
	}

	return false;
}

void WorldManager::SaveWorld()
{
	std::thread([&]()
		{
			instance->SaveWorldNow();
		}).detach();
}


Chunk* WorldManager::GetChunk(float x, float z)
{
	if (!isRegionLoaded(x, z))
		return nullptr;

	Region& r = GetRegion(x, z);

	if (r.chunks.size() == 0)
		return nullptr;



	for (auto& c : r.chunks)
	{
		if (c->IsInChunk(x, z))
			return c;
	}

	return nullptr;

}

Data::Chunk WorldManager::GetChunkData(float x, float z)
{
	if (!isRegionLoaded(x, z))
		return Data::Chunk();

	Region& r = GetRegion(x, z);

	if (r.chunks.size() == 0)
		return Data::Chunk();

	return r.GetChunkData(x, z);
}

glm::vec3 WorldManager::GetPlayerPosition()
{
	return glm::vec3(_world.p.x, _world.p.y, _world.p.z);
}

void WorldManager::SetPlayerPosition(glm::vec3 pos)
{
	_world.p.x = pos.x;
	_world.p.y = pos.y;
	_world.p.z = pos.z;
}

void WorldManager::SaveWorldNow()
{
	Game::instance->log->Write("Saving world...");

	// if we're still generating regions, wait

	while (_generatePool.get_tasks_running() != 0)
	{
		Game::instance->log->Write("Waiting for regions to finish... (" + std::to_string(_generatePool.get_tasks_running()) + " left)");
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
	}

	if (!std::filesystem::exists(_path))
		std::filesystem::create_directories(_path);

	zstr::ofstream os_p(_path + "/world.frim", std::ios::binary);

	msgpack::pack(os_p, _world);

	for (auto& r : regions)
	{
		_world.saveRegion(r.data);
	}
	Game::instance->log->Write("World saved to " + _path);
}

void WorldManager::DeleteWorld(Data::World w)
{
	std::filesystem::remove_all("worlds/" + w.name);
}