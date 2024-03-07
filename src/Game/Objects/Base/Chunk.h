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
    Block* blocks[16][16] = {nullptr};

    // input here has to be between 0-15 for both x and z
    Block* getBlock(int x, int z);
};

class Chunk : public GameObject
{
    unsigned int VAO, VBO, EBO;
    Texture* txp;

    std::vector<VVertex> vertices;
    std::vector<unsigned int> indices;

    void CreateFaces(Block* b);
public:

    bool IsLoaded();
    bool isRendered();

    Chunk(Texture* _txp, glm::vec3 _pos);

    Data::Chunk myData;

    std::vector<subChunk> subChunks;

    subChunk& GetSubChunk(int y);

    Data::Chunk GetChunkData();

    int GetBlock(int x, int y, int z);
    bool DoesBlockExist(int x, int y, int z);

    void ModifySubChunk(int y, Block* to);

    void RenderSubChunk(int y);
    void RenderSubChunks();

    subChunk CreateSubChunk(int y);
    Block* CreateBlock(int x, int y, int z, int id);

    void DestroySubChunk(int y);
    void DestroySubChunk(subChunk& c);
    void DestroySubChunks();
    
    void CreateSubChunks();

    void SetBuffer();

    void Init();
    void Destroy();

    void Unload();

    void Draw() override;

};

#endif