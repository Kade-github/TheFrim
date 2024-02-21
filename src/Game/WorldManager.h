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

	Region(int _startX, int _startZ, int _endX, int _endZ, Data::Region _data, std::vector<Chunk*> _chunks)
	{
		startX = _startX;
		startZ = _startZ;
		endX = _endX;
		endZ = _endZ;
		data = _data;
		chunks = _chunks;
	}

	Region() = default;

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

	std::thread _generateThread;

	Data::World _world;
public:
	static WorldManager* instance;

	std::vector<Region> regions;

	int renderedChunks = 0;
	Texture* texturePack;

	std::string name;

	WorldManager(std::string worldPath, Texture* _tp);

	bool IsRegionLoaded(int x, int z, int endX, int endZ);

	Data::Region GetRegion(int x, int z, int endX, int endZ);
	Data::Region GetRegion(int x, int z);

	void UnloadRegion(Region& r);
	void LoadRegion(int x, int z, int endX, int endZ);

	void ShouldLoad(int x, int z);
	void GenerateRegion(int x, int z);

	std::vector<Chunk*> CreateChunksInRegion(Data::Region& r);

	void LoadChunks();

	void CreateWorld();
	void LoadWorld();
	void SaveWorld();
	void SaveWorldNow();

	void RenderChunks();
};

#endif