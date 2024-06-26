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
    Block* blocks[16][16] = {nullptr};

    // input here has to be between 0-15 for both x and z
    Block* getBlock(int x, int z);
};

class Chunk : public GameObject
{
    unsigned int VAO = -1, VBO = -1, EBO = -1;

    unsigned int TRANSPARENTVAO = -1, TRANSPARENTVBO = -1, TRANSPARENTEBO = -1;

    unsigned int SHADOWVAO = -1, SHADOWVBO = -1, SHADOWEBO = -1;


    Texture* txp;

    std::vector<GameObject::VVertex> vertices;
    std::vector<unsigned int> indices;

    std::vector<GameObject::VVertex> transparentVertices;
    std::vector<unsigned int> transparentIndices;

    std::vector<GameObject::VVertex> shadowVertices;
    std::vector<unsigned int> shadowIndices;

    std::deque<Block*> models;
    std::deque<Model> modelsPresent;
    void CreateFaces(Block* b);

    void ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal);
    bool keepUpdating = false;

    int shadowSize = 0;
    int transparentSize = 0;
    int size = 0;
public:
    bool inited = false;
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

    std::deque<subChunk> subChunks;

    subChunk& GetSubChunk(int y);

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

    void PlaceBlock(float x, float y, float z, Block* b);

    void RenderSubChunk(subChunk& c);
    void RenderSubChunks();

    void RenderSubChunkShadow(subChunk& c);
    void RenderSubChunksShadow();

    subChunk CreateSubChunk(int y);
    Block* CreateBlock(int x, int y, int z, int id, Data::BlockData data);

    void DestroySubChunk(int y);
    void DestroySubChunk(subChunk& c);
    void DestroySubChunks();
    
    void CreateSubChunks();

    void SetBuffer();
    void SetTransparentBuffer();
    void SetShadowBuffer();

    void Init();
    void Destroy();

    void Unload();

    void DrawRegular();
    void DrawModels();
    void DrawTransparent();
    void DrawShadows();

    void UpdateChunk(int tick);

};

#endif