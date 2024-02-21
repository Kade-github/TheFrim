#ifndef _WORLD_MANAGER_H
#define _WORLD_MANAGER_H

#include "Objects/Base/Chunk.h"
#include <BS_thread_pool.hpp>

class WorldManager
{
	BS::thread_pool _generatePool;
	std::string _path;

	std::thread _generateThread;

	Data::World _world;
public:
	static WorldManager* instance;

	std::vector<Chunk*> loadedChunks;
	int renderedChunks = 0;

	std::function<void(Chunk*)> chunkGenerated;

	Texture* texturePack;

	std::string name;

	std::vector<Chunk*> chunks;

	WorldManager(std::string worldPath, Texture* _tp, std::function<void(Chunk*)> onGenerated);

	void CreateChunk(Chunk* pC);

	Chunk* GenerateChunk(int x, int z);
	void EdgeCheck(Chunk* c);
	void GenerateMeshes();

	Data::Chunk GetChunk(int x, int z);
	Chunk* GetLoadedChunk(int x, int z);

	void CreateWorld();
	void LoadWorld();
	void SaveWorld();


	void UploadChunks();
};

#endif