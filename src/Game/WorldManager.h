#ifndef _WORLD_MANAGER_H
#define _WORLD_MANAGER_H

#include "Objects/Base/Chunk.h"
#include <BS_thread_pool.hpp>

class WorldManager
{
	BS::thread_pool _generatePool;
	std::string _path;

	std::vector<Chunk*> loadedChunks;

	std::thread _edgeThread;
	std::thread _generateThread;

	Data::World _world;
public:
	static WorldManager* instance;

	std::function<void(Chunk*)> chunkGenerated;

	Texture* texturePack;

	std::string name;

	std::vector<Chunk*> chunks;

	WorldManager(std::string worldPath, Texture* _tp, std::function<void(Chunk*)> onGenerated);

	void CreateChunk(Chunk* pC);

	Data::Chunk* GenerateChunk(int x, int z);
	void GenerateEdgeChunks();
	void GenerateMeshes();

	Data::Chunk* GetChunk(int x, int z);
	Chunk* GetLoadedChunk(int x, int z);

	void CreateWorld();
	void LoadWorld();
	void SaveWorld();


	void UploadChunks();
};

#endif