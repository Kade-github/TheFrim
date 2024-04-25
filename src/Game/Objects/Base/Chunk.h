#ifndef _CHUNK_H
#define _CHUNK_H
#include <Objects/GameObject.h>
#include <OpenGL/Texture.h>
#include "../../Data/World.h"

#include "Block.h"

// split the chunk into subchunks by y axis.

class subChunk {
public:
    int y = -1;
    std::shared_ptr<Block> blocks[16][16] = {nullptr};

    // input here has to be between 0-15 for both x and z
    std::shared_ptr<Block> getBlock(int x, int z);
};

class Chunk : public GameObject
{
    unsigned int VAO, VBO, EBO;

    unsigned int TRANSPARENTVAO, TRANSPARENTVBO, TRANSPARENTEBO;

    unsigned int SHADOWVAO, SHADOWVBO, SHADOWEBO;
    Texture* txp;

    std::vector<GameObject::VVertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<GameObject::VVertex> transparentVertices;
    std::vector<unsigned int> transparentIndices;

    std::vector<GameObject::VVertex> shadowVertices;
    std::vector<unsigned int> shadowIndices;

    void CreateFaces(Block* b);

    void ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal);
    bool keepUpdating = false;

    int shadowSize = 0;
    int size = 0;
public:
    bool isBeingLoaded = false;
    bool isShadowLoaded = false;

    bool modified = false;

    bool pleaseRender = false;

    bool isLoaded = false;
    bool isRendered = false;
    Chunk(Texture* _txp, glm::vec3 _pos);

    Data::Chunk myData;

    Chunk* left = nullptr;
    Chunk* right = nullptr;
    Chunk* front = nullptr;
    Chunk* back = nullptr;

    std::vector<std::shared_ptr<subChunk>> subChunks;

    std::shared_ptr<subChunk> GetSubChunk(int y);

    Data::Chunk GetChunkData();

    glm::vec3 WorldToChunk(glm::vec3 pos);

    bool IsInChunk(float x, float z);

    int GetBlock(float x, float y, float z);

    int GetHighestBlock(float x, float z, bool water = false);

    int GetBlockNoCheck(float x, float y, float z);

    int GetBlockRaw(float x, float y, float z);

    // checks current chunk, and if needed; checks chunks around it.
    bool InterchunkDoesBlockExist(float x, float y, float z);

    int GetBlockInterchunk(float x, float y, float z);

    bool DoesBlockExist(float x, float y, float z);

    void CreateOtherSubchunks(glm::vec3 _w);

    void ModifyBlock(float x, float y, float z, int id);

    void PlaceBlock(float x, float y, float z, std::shared_ptr<Block>b);

    void RenderSubChunk(std::shared_ptr<subChunk> c);
    void RenderSubChunks();

    void RenderSubChunkShadow(std::shared_ptr<subChunk> c);
    void RenderSubChunksShadow();

    std::shared_ptr<subChunk> CreateSubChunk(int y);
    std::shared_ptr<Block> CreateBlock(int x, int y, int z, int id, Data::BlockData data);

    void DestroySubChunk(int y);
    void DestroySubChunk(std::shared_ptr<subChunk> c);
    void DestroySubChunks();
    
    void CreateSubChunks();

    void SetBuffer();
    void SetTransparentBuffer();
    void SetShadowBuffer();

    void Init();
    void Destroy();

    void Unload();

    void DrawRegular();
    void DrawTransparent();
    void DrawShadows();

    void UpdateChunk(int tick);

};

#endif