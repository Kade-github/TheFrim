#include "World.h"
#include <iostream>
#include <filesystem>
#include <Helpers/StringTools.h>
#include "../../zstr/src/zstr.hpp"
#include <PerlinNoise.hpp>

siv::PerlinNoise perlin;

std::mutex m;

Data::Chunk Data::Region::getChunk(int x, int z)
{
	std::lock_guard<std::mutex> lock(m);
	int realX = (x - startX) / 16;
	int realZ = (z - startZ) / 16;

	if (realX < 0 || realX > 4 || realZ < 0 || realZ > 4)
		return Chunk();

	if (chunks.size() == 0)
		return Chunk();

	if (chunks[realX].size() == 0)
		return Chunk();

	return chunks[realX][realZ];
}

Data::Chunk* Data::Region::getChunkPtr(int x, int z)
{
	std::lock_guard<std::mutex> lock(m);
	int realX = (x - startX) / 16;
	int realZ = (z - startZ) / 16;

	if (realX < 0 || realX > 4 || realZ < 0 || realZ > 4)
		return nullptr;

	if (chunks.size() == 0)
		return nullptr;

	if (chunks[realX].size() == 0)
		return nullptr;

	return &chunks[realX][realZ];
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

void Data::World::parseSeed()
{
	int _seed = 0;

	for (int i = 0; i < seed.size(); i++)
	{
		_seed += (int)seed[i];
	}

	seedNum = _seed;

	perlin.reseed(seedNum);
}

Data::Region Data::World::getRegion(int x, int z)
{
	int chunkSize = 16;

	int width = (chunkSize * 5);
	int depth = (chunkSize * 5);

	for (auto& stored : storedRegions)
	{
		std::vector<std::string> parts = StringTools::Split(stored, "_");

		if (parts.size() != 5)
			continue;

		int _x = std::stoi(parts[1]);
		int _z = std::stoi(parts[2]);
		int _endX = std::stoi(parts[3]);
		int _endZ = std::stoi(parts[4]);

		if (x == _x && z == _z && x + width == _endX && z + depth == _endZ)
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
				{
					r.chunks[i][j].isGenerated = true;
				}

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
	int depth = (chunkSize * 5);

	r.startX = x * width;
	r.startZ = z * depth;
	r.endX = r.startX + width;
	r.endZ = r.startZ + depth;

	for (int _x = r.startX; _x < r.endX; _x += 16)
	{
		for (int _z = r.startZ; _z < r.endZ; _z += 16)
		{
			r.addChunk(r.generateChunk(_x,_z));
		}
	}
	return r;
	
}

void Data::World::saveRegion(Region r)
{
	std::string name = "r_" + std::to_string(r.startX) + "_" + std::to_string(r.startZ) + "_" + std::to_string(r.endX) + "_" + std::to_string(r.endZ) + ".r";

	try
	{
		zstr::ofstream file(_path + "/" + name, std::ios_base::binary);

		msgpack::pack(file, r);

		file.close();
	}
	catch (const std::exception& e)
	{
		std::cout << "Failed to open " << name << " " << e.what() << std::endl;
	}
}

Data::Chunk Data::Region::generateChunk(int x, int z)
{
	Chunk chunk;

	chunk.x = x;
	chunk.z = z;

	// set everything to 0

	for (int _x = 0; _x < 16; _x++)
	{
		for (int _z = 0; _z < 16; _z++)
		{
			for (int _y = 0; _y < 256; _y++)
			{
				chunk.blocks[_x][_z][_y] = 0;
			}
		}
	}

	float scale = 0.002;

	for (int _x = 0; _x < 16; _x++)
	{
		for (int _z = 0; _z < 16; _z++)
		{
			int worldX = (_x + x);
			int worldZ = (_z + z);

			const double noise = perlin.normalizedOctave2D(worldX * scale, worldZ * scale, 4, 0.5);

			int rY = 128 + (int)((noise * 100));

			if (rY < 0)
				rY = 0;

			if (rY > 255)
				rY = 255;

			for (int _y = rY; _y > -1; _y--)
			{
				if (_y == rY)
					chunk.blocks[_x][_z][_y] = 2;
				else
					chunk.blocks[_x][_z][_y] = 1;
			}
		}
	}

	chunk.isGenerated = true;

	return chunk;
}