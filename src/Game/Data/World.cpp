#include "Structures/Tree.h"
#include "Structures/Ruins.h"

#include <iostream>
#include <filesystem>
#include <Helpers/StringTools.h>
#include "../../zstr/src/zstr.hpp"
#include <PerlinNoise.hpp>
#include "../Objects/Base/Block.h"

#include <Game.h>

siv::PerlinNoise perlin;
siv::PerlinNoise cavePerlin;
siv::PerlinNoise orePerlin;

std::mutex m;

int staticWaterLevel = 0;
int staticRandomAdvancement = 0;
int staticSeed = 0;

std::minstd_rand0 r;

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

bool Data::Region::doesBlockExist(int x, int y, int z)
{
	Chunk* c = getChunkPtr(x, z);

	if (c == nullptr)
		return false;

	return c->bChunk.blocks[x - c->x][z - c->z][y] > 0;
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
						if (c.bChunk.blocks[_x][_z][_y] == type)
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

int getRandom(int min, int max)
{
	staticRandomAdvancement++;
	r.seed(staticSeed + staticRandomAdvancement);
	return r() % (max - min + 1) + min;	
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

	cavePerlin.reseed(seedNum + 1);

	orePerlin.reseed(seedNum + 2);

	srand(seedNum);

	staticRandomAdvancement = randomAdvancement;

	r = std::minstd_rand0(seedNum);

	staticSeed = seedNum;

	if (waterLevel == -1)
		waterLevel = getRandom(80, 125);

	staticWaterLevel = waterLevel;

	Game::instance->log->Write("Seed: " + std::to_string(seedNum));
	Game::instance->log->Write("Water Level: " + std::to_string(waterLevel));
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
	randomAdvancement = staticRandomAdvancement;

	std::string name = "r_" + std::to_string(r.startX) + "_" + std::to_string(r.startZ) + "_" + std::to_string(r.endX) + "_" + std::to_string(r.endZ) + ".r";

	Game::instance->log->Write("Saving " + name);

	try
	{
		zstr::ofstream file(_path + "/" + name, std::ios_base::binary);

		msgpack::pack(file, r);

		file.close();
	}
	catch (const std::exception& e)
	{
		Game::instance->log->Write("Error saving region: " + std::string(e.what()));
	}
}

void Data::Chunk::placeStone(int x, int y, int z, float multiplier)
{
	float m = multiplier;

	if (y <= 25)
		m *= 1.5f;
	else if (y <= 40)
		m *= 1.25;

	double coalNoise = std::abs((orePerlin.normalizedOctave3D(x * 0.002, y * 0.002, z * 0.002, 10, 0.5)) * m);
	double ironNoise = std::abs((orePerlin.normalizedOctave3D(x * 0.002, y * 0.02, z * 0.002, 10, 0.5)) * m);
	double diamondNoise = std::abs((orePerlin.normalizedOctave3D(x * 0.002, y * 0.002, z * 0.002, 10, 0.5)) * m);
	double goldNoise = std::abs((orePerlin.normalizedOctave3D(x * 0.002, y * 0.002, z * 0.002, 10, 0.5)) * m);

	if (coalNoise >= 0.4 && amountOfBlockInRadius(x, y, z, COAL_ORE, 16) < 8)
	{
		placeBlock(x, y, z, COAL_ORE);
	}
	else if (ironNoise >= 0.5 && amountOfBlockInRadius(x, y, z, IRON_ORE, 16) < 4)
		placeBlock(x, y, z, IRON_ORE);
	else if (diamondNoise >= 0.5 && y <= 40 && amountOfBlockInRadius(x, y, z, DIAMOND_ORE, 24) < 4)
		placeBlock(x, y, z, DIAMOND_ORE);
	else if (goldNoise >= 0.5 && y <= 50 && amountOfBlockInRadius(x, y, z, GOLD_ORE, 16) < 4)
		placeBlock(x, y, z, GOLD_ORE);
	else
		placeBlock(x, y, z, STONE);
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
				chunk.bChunk.blocks[_x][_z][_y] = 0;
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

			const double noise = perlin.normalizedOctave2D(worldX * scale, worldZ * scale, 6, 1.0);

			int rY = (CHUNK_HEIGHT / 2) + (int)((noise * 100));

			if (rY < 0)
				rY = 0;

			if (rY > CHUNK_HEIGHT - 1)
				rY = CHUNK_HEIGHT - 1;

			bool inCave = false;

			for (int _y = rY; _y > -1; _y--)
			{
				// check if we're a cave

				double caveNoise = cavePerlin.normalizedOctave3D(worldX * scale, _y * scale, worldZ * scale, 6, 1.0);

				if (_y <= 128)
					caveNoise *= 2.0;
				if (caveNoise <= 0.4)
					caveNoise = 0;


				if (caveNoise <= 0)
				{

					if (_y == rY) // grass or sand
					{
						if (rY <= staticWaterLevel)
							chunk.bChunk.blocks[_x][_z][_y] = SAND;
						else
							chunk.bChunk.blocks[_x][_z][_y] = GRASS;
					}
					else if (_y > rY - 5) // dirt or sand
					{
						if (rY <= staticWaterLevel)
							chunk.bChunk.blocks[_x][_z][_y] = SAND;
						else
							chunk.bChunk.blocks[_x][_z][_y] = DIRT;
					}
					else // stone
						chunk.placeStone(_x, _y, _z, inCave ? 3.0f : 1.0f);
				}
				else
				{
					for (int i = 0; i < 3; i++)
						chunk.placeStone(_x, (rY - 3) - i, _z);

					chunk.bChunk.blocks[_x][_z][_y] = 0;
					inCave = true;
				}

				if (_y == 0)
					chunk.bChunk.blocks[_x][_z][_y] = BEDROCK;
			}

			if (rY < staticWaterLevel)
			{
				for (int _y = staticWaterLevel; _y > rY; _y--)
				{
					if (chunk.bChunk.blocks[_x][_z][_y] != 0)
						continue;

					chunk.bChunk.blocks[_x][_z][_y] = WATER;
				}
			}
		}
	}

	chunk.isGenerated = true;

	return chunk;
}

void Data::Region::generateStructures()
{
	static Data::Tree struct_tree = Data::Tree();
	static Data::Ruins struct_ruins = Data::Ruins();

	for (int i = 0; i < REGION_SIZE; i++)
	{
		for (int j = 0; j < REGION_SIZE; j++)
		{
			Chunk& c = chunks[i][j];

			if (!c.isGenerated)
				continue;

			bool madeRuin = false;

			// ruins

			int randomX = getRandom(0, CHUNK_SIZE - 1);
			int randomZ = getRandom(0, CHUNK_SIZE - 1);

			int randomY = c.getHighestBlock(randomX, randomZ);

			if (getRandom(0, 100) > 65) // 45% chance of it being under ground
				randomY += getRandom(0, 60);

			if (getRandom(0, 1000) < 50) // 5% chance of a ruin
			{
				struct_ruins.Create(c.x + randomX, c.z + randomZ, randomY, c, this);
				madeRuin = true;
			}

			if (madeRuin)
				continue;

			for (int _x = 0; _x < CHUNK_SIZE; _x++)
			{
				for (int _z = 0; _z < CHUNK_SIZE; _z++)
				{
					for (int _y = CHUNK_HEIGHT - 1; _y > -1; _y--)
					{
						if (c.bChunk.blocks[_x][_z][_y] <= 0)
							continue;

						if (_y <= staticWaterLevel)
							continue;

						// trees

						if (c.bChunk.blocks[_x][_z][_y] == GRASS)
						{
							int _rx = c.x + _x;
							int _rz = c.z + _z;

							if (getRandom(0,100) < 2)
							{
								struct_tree.Create(_rx,_rz,_y - 1, c, this);
							}
						}

						
					}
				}
			}
		}
	}
}
