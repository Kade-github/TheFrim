#ifndef _WORLD_MANAGER_H
#define _WORLD_MANAGER_H

#include "Objects/Base/Chunk.h"
#include <BS_thread_pool.hpp>

class Region {
public:
	int startX = 0, startZ = 0;
	int endX = 0, endZ = 0;

	bool loaded = true;

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

	Data::Chunk GetChunkData(int x, int z)
	{
		return data.getChunk(x, z);
	}

	// equals operator

	bool operator==(const Region& r) const
	{
		return startX == r.startX && startZ == r.startZ && endX == r.endX && endZ == r.endZ;
	}
};

class WorldManager
{
	BS::thread_pool _generatePool;
	std::string _path;

	Data::World _world;
	std::vector<Region> _generatedRegions;
public:
	static WorldManager* instance;

	std::vector<Region> regions;

	Texture* texturePack;

	std::string name;

	WorldManager(std::string name, Texture* _tp, std::string _seed);

	WorldManager(Data::World _world, Texture* _tp);

	WorldManager(Texture* _tp);

	static std::vector<Data::World> GetWorlds();

	void CreateChunks(Region& r);

	void CheckGeneratedRegions();

	bool IsRegionGenerated(int x, int z);
	bool isRegionLoaded(int x, int z);
	void LoadRegion(int x, int z);

	Region& GetRegion(int x, int z);
	Chunk* GetChunk(int x, int z);

	void GenerateRegion(int x, int z);

	void CreateWorld(std::string _seed, std::string _name);
	void LoadWorldFile();
	void LoadWorld();
	void SaveWorld();
	void SaveWorldNow();

};

#endif