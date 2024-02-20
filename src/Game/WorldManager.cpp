#include "WorldManager.h"
#include <Game.h>
#include <fstream>
#include <zstr.hpp>

#ifdef _WIN32
#include <filesystem>
#else
#include <experimental/filesystem>
using namespace Data;
namespace std
{
	namespace filesystem = experimental::filesystem;
}
#endif

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

	_edgeThread = std::thread([&]()
		{
			while (true)
			{
				instance->GenerateEdgeChunks();
				std::this_thread::sleep_for(std::chrono::milliseconds(400));
			}
		});

	_edgeThread.detach();

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
			Data::Chunk& c = _toCreate.back();

			c.isGenerated = true;

			Chunk* ch = new Chunk(glm::vec3(c.x, c.y, c.z), texturePack);
			_toUpload.push_back(ch);
			_toCreate.pop_back();
		}
	}

	Camera* camera = Game::instance->GetCamera();

	// figure out which chunks should be loaded

	loadedChunks = {};

	int i = 0;
	for (auto& c : chunks)
	{
		glm::vec3 r = c->position;
		r.y = 128;

		float diff = std::abs(glm::distance(r, camera->position));

		float angle = atan2(camera->position.z - r.z, camera->position.x - r.x);

		if (diff < camera->cameraFar && angle < 180 && angle > -180)
		{

			if (!c->isLoaded)
				c->GenerateMesh(_world.chunks[i]);
		}
		else if (c->isLoaded)
			c->UnloadMesh();

		if (c->isLoaded)
			loadedChunks.push_back(c);


		i++;
	}

}

void WorldManager::GenerateChunk(int x, int z)
{
	Data::World* w = &_world;

	Game::instance->log->Write("Generating chunk at " + std::to_string(x) + " " + std::to_string(z));

	_generatePool.detach_task([x,z,w]()
		{
			Data::Chunk c = w->generateChunk(x, z);

			c.isGenerated = true;

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
		c.isGenerated = true;
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

void WorldManager::GenerateEdgeChunks()
{
	for (auto& c : loadedChunks)
	{
		Data::Chunk forward;
		Data::Chunk back;
		Data::Chunk left;
		Data::Chunk right;
		{
			mtx.lock();
			forward = GetChunk(c->position.x, c->position.z + 16);
			back = GetChunk(c->position.x, c->position.z - 16);
			left = GetChunk(c->position.x - 16, c->position.z);
			right = GetChunk(c->position.x + 16, c->position.z);
			mtx.unlock();
			if (!forward.isGenerated)
				GenerateChunk(c->position.x, c->position.z + 16);

			if (!back.isGenerated)
				GenerateChunk(c->position.x, c->position.z - 16);

			if (!left.isGenerated)
				GenerateChunk(c->position.x - 16, c->position.z);

			if (!right.isGenerated)
				GenerateChunk(c->position.x + 16, c->position.z);
		}
	}
}

Data::Chunk WorldManager::GetChunk(int x, int z)
{
	for (auto& c : _world.chunks)
	{
		if (c.isGenerated)
			if (c.x == x && c.z == z)
				return c;
	}

	return Data::Chunk();
}
