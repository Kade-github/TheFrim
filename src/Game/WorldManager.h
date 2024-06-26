#ifndef _WORLD_MANAGER_H
#define _WORLD_MANAGER_H

#include "Objects/Base/Chunk.h"
#include <BS_thread_pool.hpp>

class Region {
public:
	int startX = 0, startZ = 0;
	int endX = 0, endZ = 0;

	bool loaded = true;

	bool shouldUnload = false;

	Data::Region data;

	std::vector<Chunk*> chunks;

	Region(Data::Region _data, std::vector<Chunk*> _chunks)
	{
		startX = _data.startX;
		startZ = _data.startZ;
		endX = _data.endX;
		endZ = _data.endZ;
		data = _data;
		chunks = _chunks;
	}

	Region() = default;

	~Region()
	{
		chunks.clear();
	}

	Data::Chunk GetChunkData(int x, int z)
	{
		return data.getChunk(x, z);
	}

	Data::Chunk* GetChunkDataRef(int x, int z)
	{
		return data.getChunkPtr(x, z);
	}

	// equals operator

	bool operator==(const Region& r) const
	{
		return startX == r.startX && startZ == r.startZ && endX == r.endX && endZ == r.endZ;
	}
};

class WorldManager
{

	std::deque<Region> _generatedRegions;
public:
	Data::World _world;
	std::mutex generateMutex;
	std::string _path;
	BS::thread_pool _generatePool;
	static WorldManager* instance;

	std::deque<Region> regions;

	Texture* texturePack;

	std::string name;

	WorldManager(std::string name, Texture* _tp, std::string _seed);

	WorldManager(Data::World _world, Texture* _tp);

	WorldManager(Texture* _tp);

	static std::vector<Data::World> GetWorlds();

	void CreateChunks(Region& r);

	void CheckGeneratedRegions();

	~WorldManager();

	bool IsRegionGenerated(int x, int z);
	bool isRegionLoaded(float x, float z);
	void LoadRegion(int x, int z);
	Region& GetOrLoadRegion(int x, int z);

	void SaveRegion(int x, int z);
	void SaveRegion(Region& r);

	Region& GetRegion(float x, float z);
	Chunk* GetChunk(float x, float z);
	Data::Chunk GetChunkData(float x, float z);

	glm::vec3 GetPlayerPosition();
	void SetPlayerPosition(glm::vec3 pos);

	void GenerateRegion(int x, int z);

	void CreateWorld(std::string _seed, std::string _name);
	void LoadWorldFile();
	void LoadWorld();
	void SaveWorld();
	void SaveWorldNow();

	void DeleteWorld(Data::World w);

};

#endif