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
		LoadWorld();

	_generatePool.reset(6);
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

	_generatePool.detach_task([&]() {
		GenerateRegion(0, 0);
		GenerateRegion(1, 0);
		GenerateRegion(-1, 0);
		GenerateRegion(0, 1);
		GenerateRegion(0, -1);
	});

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
	int chunkSize = 16;

	int width = (chunkSize * 5);
	int depth = (chunkSize * 5);

	for (int x = r.startX; x < r.startX + width; x += chunkSize)
	{
		for (int z = r.startZ; z < r.startZ + depth; z += chunkSize)
		{
			Chunk* c = new Chunk(glm::vec3(x, 0, z), texturePack);

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
	int playerX = _world.playerX;
	int playerZ = _world.playerZ;

	// initial
	_generatePool.detach_task([&]() {
		int x = playerX / 80;
		int z = playerZ / 80;
		instance->LoadRegion(x,z);
	});
	
	// load surrounding regions

	_generatePool.detach_task([&]() {
		int x = playerX / 80;
		int z = playerZ / 80;
		instance->LoadRegion(x + 1, z);
	});

	_generatePool.detach_task([&]() {
		int x = playerX / 80;
		int z = playerZ / 80;
		instance->LoadRegion(x - 1, z);
	});

	_generatePool.detach_task([&]() {
		int x = playerX / 80;
		int z = playerZ / 80;
		instance->LoadRegion(x, z + 1);
	});

	_generatePool.detach_task([&]() {
		int x = playerX / 80;
		int z = playerZ / 80;
		instance->LoadRegion(x, z - 1);
	});
}

Region& WorldManager::GetRegion(int x, int z)
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

bool WorldManager::isRegionLoaded(int x, int z)
{
	int chunkSize = 16;

	int width = (chunkSize * 5);
	int depth = (chunkSize * 5);

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


Chunk* WorldManager::GetChunk(int x, int z)
{
	if (!isRegionLoaded(x, z))
		return nullptr;

	Region& r = GetRegion(x, z);

	if (r.chunks.size() == 0)
		return nullptr;

	for (auto& c : r.chunks)
	{
		if ((int)c->position.x <= x && (int)c->position.z <= z && (int)c->position.x + 16 > x && (int)c->position.z + 16 > z)
			return c;
	}

	return nullptr;

}

void WorldManager::SaveWorldNow()
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
	Game::instance->log->Write("World saved to " + _path);
}