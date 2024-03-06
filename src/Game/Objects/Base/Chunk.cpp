#include "../../WorldManager.h"

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

void Draw()
{
    
}