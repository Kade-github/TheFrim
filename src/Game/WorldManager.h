#ifndef _WORLD_MANAGER_H
#define _WORLD_MANAGER_H

#include "Objects/Base/Chunk.h"
#include <BS_thread_pool.hpp>

class WorldManager
{
	BS::thread_pool _generatePool;
	std::string _path;
	std::vector<Chunk*> _toUpload;
	std::vector<Data::Chunk> _toCreate;

	std::vector<Chunk*> loadedChunks;
	std::thread _edgeThread;

	Data::World _world;
public:
	static WorldManager* instance;

	std::function<void(Chunk*)> chunkGenerated;

	Texture* texturePack;

	std::string name;

	std::vector<Chunk*> chunks;

	WorldManager(std::string worldPath, Texture* _tp, std::function<void(Chunk*)> onGenerated);

	void GenerateChunk(int x, int z);
	void GenerateEdgeChunks();

	Data::Chunk GetChunk(int x, int z);

	void CreateWorld();
	void LoadWorld();
	void SaveWorld();


	void UploadChunks();
};

#endif