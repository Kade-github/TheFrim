#include "World.h"
#include <iostream>
#include <filesystem>
#include <Helpers/StringTools.h>
#include "../../zstr/src/zstr.hpp"
#include <PerlinNoise.hpp>
#include "../Objects/Base/Block.h"

siv::PerlinNoise perlin;

std::mutex m;

Data::Chunk Data::Region::getChunk(int x, int z)
{
	std::lock_guard<std::mutex> lock(m);
	int realX = (x - startX) / CHUNK_SIZE;
	int realZ = (z - startZ) / CHUNK_SIZE;

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
	int realX = (x - startX) / CHUNK_SIZE;
	int realZ = (z - startZ) / CHUNK_SIZE;

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

	int realX = (c.x - startX) / CHUNK_SIZE;
	int realZ = (c.z - startZ) / CHUNK_SIZE;

	if (realX < 0 || realX > 4 || realZ < 0 || realZ > 4)
	{
		return;
	}

	chunks[realX][realZ] = c;
}

void Data::Region::freePlace(int x, int y, int z, int type)
{
	Chunk* c = getChunkPtr(x, z);

	if (c == nullptr)
		return;

	c->placeBlock(x - c->x, y, z - c->z, type);
}

bool Data::Region::doesBlockExistInRange(int x, int y, int z, int type, int range)
{
	std::lock_guard<std::mutex> lock(m);

	for (int i = 0; i < REGION_SIZE; i++)
	{
		for (int j = 0; j < REGION_SIZE; j++)
		{
			Chunk& c = chunks[i][j];

			for (int _x = 0; _x < CHUNK_SIZE; _x++)
			{
				for (int _z = 0; _z < CHUNK_SIZE; _z++)
				{
					for (int _y = 0; _y < CHUNK_HEIGHT; _y++)
					{
						if (c.blocks[_x][_z][_y] == type)
						{
							if (abs(c.x + _x - x) < range && abs(c.z + _z - z) < range && abs(_y - y) < range)
								return true;
						}
					}
				}
			}
		}
	}

	return false;
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

	srand(seedNum);
}

Data::Region Data::World::getRegion(int x, int z)
{
	int width = (CHUNK_SIZE * REGION_SIZE);
	int depth = (CHUNK_SIZE * REGION_SIZE);

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

			for (int i = 0; i < REGION_SIZE; i++)
				for (int j = 0; j < REGION_SIZE; j++)
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

	int width = (CHUNK_SIZE * REGION_SIZE);
	int depth = (CHUNK_SIZE * REGION_SIZE);

	r.startX = x * width;
	r.startZ = z * depth;
	r.endX = r.startX + width;
	r.endZ = r.startZ + depth;

	for (int _x = r.startX; _x < r.endX; _x += CHUNK_SIZE)
	{
		for (int _z = r.startZ; _z < r.endZ; _z += CHUNK_SIZE)
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

	for (int _x = 0; _x < CHUNK_SIZE; _x++)
	{
		for (int _z = 0; _z < CHUNK_SIZE; _z++)
		{
			for (int _y = 0; _y < CHUNK_HEIGHT; _y++)
			{
				chunk.blocks[_x][_z][_y] = 0;
			}
		}
	}

	float scale = 0.002;

	for (int _x = 0; _x < CHUNK_SIZE; _x++)
	{
		for (int _z = 0; _z < CHUNK_SIZE; _z++)
		{
			int worldX = (_x + x);
			int worldZ = (_z + z);

			const double noise = perlin.normalizedOctave2D(worldX * scale, worldZ * scale, 4, 0.5);

			int rY = (CHUNK_HEIGHT / 2) + (int)((noise * 100));

			if (rY < 0)
				rY = 0;

			if (rY > CHUNK_HEIGHT - 1)
				rY = CHUNK_HEIGHT - 1;

			for (int _y = rY; _y > -1; _y--)
			{
				if (_y == rY) // grass
					chunk.blocks[_x][_z][_y] = GRASS;
				else if (_y > rY - 5) // dirt
					chunk.blocks[_x][_z][_y] = DIRT;
				else // stone
					chunk.blocks[_x][_z][_y] = STONE;
			}
		}
	}

	chunk.isGenerated = true;

	return chunk;
}

void Data::Region::generateStructures()
{
	for (int i = 0; i < REGION_SIZE; i++)
	{
		for (int j = 0; j < REGION_SIZE; j++)
		{
			Chunk& c = chunks[i][j];

			if (!c.isGenerated)
				continue;

			for (int _x = 0; _x < CHUNK_SIZE; _x++)
			{
				for (int _z = 0; _z < CHUNK_SIZE; _z++)
				{
					for (int _y = CHUNK_HEIGHT - 1; _y > -1; _y--)
					{
						if (c.blocks[_x][_z][_y] <= 0)
							continue;

						// trees

						if (c.blocks[_x][_z][_y] == GRASS)
						{
							if (rand() % 100 < 5)
							{
								int height = rand() % 5 + 6;

								if (height < 4)
									height = 4;

								if (height > 6)
									height = 6;

								// check if it can be placed

								bool cantPlace = doesBlockExistInRange(c.x + _x, _y, c.z + _z, WOOD, 7);

								if (cantPlace)
									continue;

								// check if we're on the region border

								if (c.x + _x <= startX + 2 || c.x + _x >= endX - 2 || c.z + _z <= startZ + 2 || c.z + _z >= endZ - 2)
									continue;

								for (int i = 0; i < height; i++)
								{
									c.placeBlock(_x, _y + i, _z, WOOD);
								}

								int _rx = c.x + _x;
								int _rz = c.z + _z;

								freePlace(_rx, _y + height, _rz, LEAVES);

								freePlace(_rx, _y + height, _rz + 1, LEAVES);
								freePlace(_rx, _y + height, _rz - 1, LEAVES);
								freePlace(_rx + 1, _y + height, _rz, LEAVES);
								freePlace(_rx - 1, _y + height, _rz, LEAVES);
								freePlace(_rx + 1, _y + height, _rz + 1, LEAVES);
								freePlace(_rx - 1, _y + height, _rz - 1, LEAVES);
								freePlace(_rx + 1, _y + height, _rz - 1, LEAVES);
								freePlace(_rx - 1, _y + height, _rz + 1, LEAVES);

								for (int g = 1; g < 3; g++)
								{
									for (int i = -2; i < 3; i++)
									{
										for (int j = -2; j < 3; j++)
										{
											if (i == 0 && j == 0)
												continue;

											freePlace(_rx + i, _y + height - g, _rz + j, LEAVES);
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}
