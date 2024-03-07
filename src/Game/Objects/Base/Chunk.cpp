#include "../../WorldManager.h"

#include "Blocks/Grass.h"
#include "Blocks/Dirt.h"

#define CHUNK_SIZE 16

Block* subChunk::getBlock(int x, int z)
{
    return blocks[x][z];
}

int Chunk::GetBlock(int x, int y, int z)
{
    int _x = x;
    int _z = z;
    if (x < 0)
        _x = position.x - x;
    if (x > CHUNK_SIZE - 1)
        _x = x - position.x;

    if (z < 0)
        _z = position.z - z;

    if (z > CHUNK_SIZE - 1)
        _z = z - position.z;
    
    WorldManager* wm = WorldManager::instance;
    
    // this could (and should) be optimized so it caches this, but that has to be implemented smartly.

    Data::Chunk myData = wm->GetChunkData(position.x, position.z);

    return myData.blocks[_x][_z][y];
}

bool Chunk::DoesBlockExist(int x, int y, int z)
{
    return GetBlock(x,y,z) > 0; // if its anything under 0 (thats impossible, uint etc), and if its 0 it's air.
}

void Chunk::ModifySubChunk(int y, Block* to)
{
    subChunk& sbc = GetSubChunk(y);

	if (sbc.y <= -1)
		return;

	int x = to->position.x - position.x;
	int z = to->position.z - position.z;

	if (x < 0 || x > 15 || z < 0 || z > 15)
		return;

	if (sbc.blocks[x][z] != nullptr)
		delete sbc.blocks[x][z];

	sbc.blocks[x][z] = to;
}

subChunk& Chunk::GetSubChunk(int y)
{
    static subChunk empty;

    for(int i = 0; i < subChunks.size(); i++)
    {
        subChunk& sbc = subChunks[i];

        if (sbc.y == y)
            return sbc;
    }
    return empty;
}

Data::Chunk Chunk::GetChunkData()
{
    Data::Chunk c;
    c.x = position.x;
    c.z = position.z;

    for (int y = 0; y < 256; y++)
    {
        subChunk& sbc = GetSubChunk(y);

        if (sbc.y <= -1) // not loaded
        {
            for (int x = 0; x < 16; x++)
            {
                for (int z = 0; z < 16; z++)
					c.blocks[x][z][y] = myData.blocks[x][z][y];
            }
            continue;
        }

        for (int x = 0; x < 16; x++)
        {
            for (int z = 0; z < 16; z++)
            {
                if (sbc.blocks[x][z] != nullptr)
					c.blocks[x][z][y] = sbc.blocks[x][z]->type;
                else
                    c.blocks[x][z][y] = 0;
			}
		}
    }

    return c;
}

subChunk Chunk::CreateSubChunk(int y)
{
    subChunk sbc;

    sbc.y = y;

    bool isOccluded = true;

    for (int x = 0; x < 16; x++)
    {
        for (int z = 0; z < 16; z++)
        {
            int id = myData.blocks[x][z][y];

            if (id <= 0)
                continue;

            if (isOccluded)
            {
                if (myData.blocks[x][z][y - 1] <= 0)
                    isOccluded = false;
                else if (myData.blocks[x][z][y + 1] <= 0)
                    isOccluded = false;
            }

            sbc.blocks[x][z] = CreateBlock(x, y, z, id);

        }
    }

    if (isOccluded)
    {
        DestroySubChunk(sbc);
        return subChunk();
    }

    return sbc;
}

Block* Chunk::CreateBlock(int x, int y, int z, int id)
{
    Block* block = nullptr;

    switch (id)
    {
    case 1:
        block = new Grass(position + glm::vec3(x, y, z));
        break;
    case 2:
        block = new Dirt(position + glm::vec3(x, y, z));
        break;
    }

	return block;
}

void Chunk::DestroySubChunk(int y)
{
    subChunk& sbc = GetSubChunk(y);

    DestroySubChunk(sbc);
}

void Chunk::DestroySubChunk(subChunk& c)
{
    for (int x = 0; x < 16; x++)
    {
        for (int z = 0; z < 16; z++)
        {
            if (c.blocks[x][z] != nullptr)
			    delete c.blocks[x][z];
		}
	}
}

void Chunk::DestroySubChunks()
{
    for (int i = 0; i < subChunks.size(); i++)
    {
		DestroySubChunk(subChunks[i].y);
	}

    subChunks.clear();
}

void Chunk::CreateSubChunks()
{
    WorldManager* wm = WorldManager::instance;

    for (int y = 255; y > -1; y--)
    {
		subChunk sbc = CreateSubChunk(y);
        if (sbc.y > -1)
            subChunks.push_back(sbc);
	}
}

void Chunk::Draw()
{
    
}