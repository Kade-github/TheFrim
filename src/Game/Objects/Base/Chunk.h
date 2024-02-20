#ifndef _CHUNK_H
#define _CHUNK_H

#include <glm/glm.hpp>
#include "Block.h"
#include <map>
#include <mutex>
#include <OpenGL/Texture.h>
#include <Objects/GameObject.h>
#include "../../Data/World.h"
#include <functional>

class Chunk : public GameObject
{
	unsigned int VBO, VAO, EBO;

	bool generatedVAO = false;

	// final vertices and indices

	std::vector<unsigned int> indices = {};

	float blockWidth = 128;
	float blockHeight = 128;

public:

	std::vector<VVertex> vertices = {};

	Texture* sheet;


	bool isLoaded = false;
	bool rendered = false;

	Data::Chunk* forwardC;
	Data::Chunk* backwardC;
	Data::Chunk* leftC;
	Data::Chunk* rightC;

	std::vector<Block*> blocks;

	void AddToDraw(std::vector<VVertex> _v, std::vector<unsigned int> _i);

	Chunk(glm::vec3 pos, Texture* _spr);

	void GenerateMesh(Data::Chunk* c);

	void UploadMesh();
	void Clean();
	void UnloadMesh();

	void Create() override;

	void Draw() override;
};

#endif