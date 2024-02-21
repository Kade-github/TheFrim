#include "World.h"
#include <iostream>
#include <filesystem>
#include <Helpers/StringTools.h>
#include "../../zstr/src/zstr.hpp"

std::mutex m;

Data::Chunk Data::Region::getChunk(int x, int z)
{
	std::lock_guard<std::mutex> lock(m);
	int realX = (x - startX) / 16;
	int realZ = (z - startZ) / 16;

	if (realX < 0 || realX > 4 || realZ < 0 || realZ > 4)
		return Chunk();

	return chunks[realX][realZ];
}

void Data::Region::addChunk(Chunk c)
{
	std::lock_guard<std::mutex> lock(m);

	int realX = (c.x - startX) / 16;
	int realZ = (c.z - startZ) / 16;

	if (realX < 0 || realX > 4 || realZ < 0 || realZ > 4)
	{
		return;
	}

	chunks[realX][realZ] = c;
}

void Data::World::scanForRegions()
{
	for (auto& file : std::filesystem::directory_iterator(_path))
	{
		if (file.path().extension() != ".r")
			continue;
		std::string name = file.path().filename().string();
		name = name.substr(0, name.find("."));

		std::vector<std::string> parts = StringTools::Split(name, "_");

		if (parts.size() != 5)
			continue;

		storedRegions.push_back(name);
	}
}

Data::Region Data::World::getRegion(int x, int z, int endX, int endZ)
{
	for (auto& stored : storedRegions)
	{
		std::vector<std::string> parts = StringTools::Split(stored, "_");

		if (parts.size() != 5)
			continue;

		int _x = std::stoi(parts[1]);
		int _z = std::stoi(parts[2]);
		int _endX = std::stoi(parts[3]);
		int _endZ = std::stoi(parts[4]);

		if (x == _x && z == _z && endX == _endX && endZ == _endZ)
		{
			zstr::ifstream f(_path + "/" + stored + ".r", std::ios::binary);

			std::stringstream buffer;

			buffer << f.rdbuf();

			msgpack::unpacked upd = msgpack::unpack(buffer.str().data(), buffer.str().size());

			f.close();

			Region r;

			upd.get().convert(r);

			for (int i = 0; i < 5; i++)
				for (int j = 0; j < 5; j++)
					r.chunks[i][j].isGenerated = true;

			return r;
		}
	}

	return Region();
}

Data::Region Data::World::generateRegion(int x, int z)
{
	Region r;

	int chunkSize = 16;

	int width = (chunkSize * 5);
	int height = (chunkSize * 5);

	r.startX = x;
	r.startZ = z;
	r.endX = x + width;
	r.endZ = z + height;

	for (int _x = x; _x < r.endX; _x += 16)
	{
		for (int _z = z; _z < r.endZ; _z += 16)
		{
			r.addChunk(r.generateChunk(_x,_z));
		}
	}

	return r;
	
}

void Data::World::saveRegion(Region r)
{
	std::string name = "r_" + std::to_string(r.startX) + "_" + std::to_string(r.startZ) + "_" + std::to_string(r.endX) + "_" + std::to_string(r.endZ) + ".r";

	zstr::ofstream file(_path + "/" + name, std::ios::binary);

	msgpack::pack(file, r);

	file.close();
}

Data::Chunk Data::Region::generateChunk(int x, int z)
{
	Chunk chunk;

	chunk.x = x;
	chunk.z = z;


	for (int _y = 0; _y < 256; _y++)
	{
		for (int _x = 0; _x < 16; _x++)
		{
			for (int _z = 0; _z < 16; _z++)
			{

				if (_y == 128)
					chunk.blocks[_y][_x][_z] = 2;
				else if (_y < 128)
					chunk.blocks[_y][_x][_z] = 1;
				else
					chunk.blocks[_y][_x][_z] = 0;
			}
		}
	}

	chunk.isGenerated = true;

	return chunk;
}