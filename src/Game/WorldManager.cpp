#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include "../zstr/src/zstr.hpp"

#include <filesystem>

WorldManager* WorldManager::instance = nullptr;

std::mutex mtx;

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
}

void WorldManager::UploadChunks()
{
	if (_toUpload.size() != 0)
	{
		std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
		if (lock.owns_lock())
		{
			Chunk* c = _toUpload.back();

			chunkGenerated(c);
			chunks.push_back(c);

			_toUpload.pop_back();
		}
	}

	if (_toCreate.size() != 0)
	{
		std::unique_lock<std::mutex> lock(mtx, std::try_to_lock);
		if (lock.owns_lock())
		{
			Data::Chunk c = _toCreate.back();

			Chunk* ch = new Chunk(glm::vec3(c.x, c.y, c.z), texturePack);
			_toUpload.push_back(ch);
			_toCreate.pop_back();
		}
	}

	Camera* camera = Game::instance->GetCamera();

	// figure out which chunks should be loaded

	int i = 0;
	for (auto& c : chunks)
	{
		glm::vec3 r = c->position;
		r.y = 128;

		float diff = std::abs(glm::distance(r, camera->position));

		float angle = atan2(camera->position.z - r.z, camera->position.x - r.x);

		if (diff < 100 && angle < 180 && angle > -180)
		{
			if (!c->isLoaded)
				c->GenerateMesh(_world.chunks[i]);
		}
		else if (c->isLoaded)
			c->UnloadMesh();
		i++;
	}
}

void WorldManager::GenerateChunk(int x, int z)
{
	Data::World* w = &_world;

	_generatePool.detach_task([x,z,w]()
		{
			Data::Chunk c = w->generateChunk(x, z);

			{
				std::lock_guard<std::mutex> lock(mtx);
				w->chunks.push_back(c);
				instance->_toCreate.push_back(c);
			}
		});
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
			GenerateChunk(x * 16, z * 16);
		}
	}

	std::thread([&]()
		{
			_generatePool.wait();

			zstr::ofstream os_p(_path, std::ios::binary);

			msgpack::pack(os_p, _world);

			Game::instance->log->Write("World saved to " + _path);
		}).detach();
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
		Data::Chunk* _c = &c;
		Chunk* ch = new Chunk(glm::vec3(_c->x, _c->y, _c->z), texturePack);
		_generatePool.detach_task([_c, ch]() {
			std::lock_guard<std::mutex> lock(mtx);
			instance->_toUpload.push_back(ch);

		});
	}
}

void WorldManager::SaveWorld()
{

}
