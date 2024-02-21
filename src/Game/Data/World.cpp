#include "World.h"
#include <iostream>
#include <filesystem>
#include "../../zstr/src/zstr.hpp"

std::mutex m;

Data::Chunk Data::Region::getChunk(int x, int z)
{
	std::lock_guard<std::mutex> lock(m);
	for (int i = 0; i < chunks.size(); i++)
	{
		if (chunks[i].x == x && chunks[i].z == z)
		{
			return chunks[i];
		}
	}
	return Chunk();
}

void Data::Region::addChunk(Chunk c)
{
	std::lock_guard<std::mutex> lock(m);
	chunks.push_back(c);
}

Data::Region Data::World::getRegion(int x, int z)
{
	for (auto& file : std::filesystem::directory_iterator(_path))
	{
		if (file.path().extension() == ".region")
		{
			std::string name = file.path().filename().string();
			name = name.substr(0, name.find("."));
			
		}
	}
	return Region();
}
