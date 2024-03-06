#ifndef _CHUNK_H
#define _CHUNK_H

#include <Objects/GameObject.h>
#include <OpenGL/Texture.h>
#include "../../Data/World.h"

#include "Block.h"

// split the chunk into subchunks by y axis.

struct subChunk {
public:
    int y = -1;
    Block* blocks[16][16];

    // input here has to be between 0-15 for both x and z
    Block* getBlock(int x, int z);
};

class Chunk : public GameObject
{
public:
    std::vector<subChunk> subChunks;

    subChunk& GetSubChunk(int y);

    int GetBlock(int x, int y, int z);
    bool DoesBlockExist(int x, int y, int z);

    void ModifySubChunk(int y, Block* to);

    void RenderSubChunk();
    void CreateSubChunk();
    
    void CreateSubChunks();

    void Draw() override;

};

#endif