#include "../../WorldManager.h"

#include <Game.h>

#include "Blocks/Grass.h"
#include "Blocks/Dirt.h"


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

	if (y < 0 || y > CHUNK_HEIGHT - 1)
		return 0;

	if (x < 0 || x > CHUNK_SIZE - 1 || z < 0 || z > CHUNK_SIZE - 1)
		return 0;

	return myData.blocks[_x][_z][y];
}

bool Chunk::DoesBlockExist(int x, int y, int z)
{
	return GetBlock(x, y, z) > 0; // if its anything under 0 (thats impossible, uint etc), and if its 0 it's air.
}

void Chunk::ModifySubChunk(int y, Block* to)
{
	subChunk& sbc = GetSubChunk(y);

	if (sbc.y <= -1)
		return;

	int x = to->position.x - position.x;
	int z = to->position.z - position.z;

	if (x < 0 || x > CHUNK_SIZE - 1 || z < 0 || z > CHUNK_SIZE - 1)
		return;

	if (sbc.blocks[x][z] != nullptr)
		delete sbc.blocks[x][z];

	sbc.blocks[x][z] = to;
}

void Chunk::RenderSubChunk(int y)
{
	subChunk& sbc = GetSubChunk(y);

	if (sbc.y <= -1)
		return;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			Block* block = sbc.getBlock(x, z);
			if (block == nullptr)
				continue;

			CreateFaces(block);
		}
	}
}

void Chunk::RenderSubChunks()
{
	vertices.clear();
	indices.clear();

	for (int i = 0; i < subChunks.size(); i++)
	{
		subChunk& sbc = subChunks[i];
		if (sbc.y <= -1)
			continue;

		RenderSubChunk(sbc.y);
	}
}

void Chunk::CreateFaces(Block* b)
{
	if (b == nullptr)
		return;

	// check adjacent blocks
	bool top = false;
	bool bottom = false;
	bool left = false;
	bool right = false;
	bool front = false;
	bool back = false;

	int x = b->position.x;
	int y = b->position.y;
	int z = b->position.z;

	// in our chunk
	if (DoesBlockExist(x, y + 1, z))
		top = true;

	if (DoesBlockExist(x, y - 1, z))
		bottom = true;

	if (x >= 0 && DoesBlockExist(x - 1, y, z))
		left = true;

	if (x <= 15 && DoesBlockExist(x + 1, y, z))
		right = true;

	if (z >= 0 && DoesBlockExist(x, y, z - 1))
		front = true;

	if (z <= 15 && DoesBlockExist(x, y, z + 1))
		back = true;

	// in adjacent chunks

	if (x == 0)
	{
		Chunk* leftChunk = WorldManager::instance->GetChunk(position.x - CHUNK_SIZE, position.z);
		if (leftChunk != nullptr)
		{
			if (leftChunk->DoesBlockExist(15, y, z))
				left = true;
		}
	}

	if (x == CHUNK_SIZE - 1)
	{
		Chunk* rightChunk = WorldManager::instance->GetChunk(position.x + CHUNK_SIZE, position.z);
		if (rightChunk != nullptr)
		{
			if (rightChunk->DoesBlockExist(0, y, z))
				right = true;
		}
	}

	if (z == 0)
	{
		Chunk* frontChunk = WorldManager::instance->GetChunk(position.x, position.z - CHUNK_SIZE);
		if (frontChunk != nullptr)
		{
			if (frontChunk->DoesBlockExist(x, y, 15))
				front = true;
		}
	}

	if (z == CHUNK_SIZE - 1)
	{
		Chunk* backChunk = WorldManager::instance->GetChunk(position.x, position.z + CHUNK_SIZE);
		if (backChunk != nullptr)
		{
			if (backChunk->DoesBlockExist(x, y, 0))
				back = true;
		}
	}

	// create faces

	if (!top)
	{
		BlockFace f = b->CreateTopFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
	}

	if (!bottom)
	{
		BlockFace f = b->CreateBottomFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
	}

	if (!left)
	{
		BlockFace f = b->CreateLeftFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
	}

	if (!right)
	{
		BlockFace f = b->CreateRightFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
	}

	if (!front)
	{
		BlockFace f = b->CreateFrontFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
	}

	if (!back)
	{
		BlockFace f = b->CreateBackFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
	}
}

bool Chunk::IsLoaded()
{
	return subChunks.size() != 0;
}

bool Chunk::isRendered()
{
	return vertices.size() != 0 && indices.size() != 0;
}

Chunk::Chunk(Texture* _txp, glm::vec3 _pos) : GameObject(_pos)
{
	position = _pos;
	txp = _txp;
}

subChunk& Chunk::GetSubChunk(int y)
{
	static subChunk empty;

	for (int i = 0; i < subChunks.size(); i++)
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

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		subChunk& sbc = GetSubChunk(y);

		if (sbc.y <= -1) // not loaded
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
					c.blocks[x][z][y] = myData.blocks[x][z][y];
			}
			continue;
		}

		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
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

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
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

	block->textureHeight = txp->height;
	block->textureWidth = txp->width;

	return block;
}

void Chunk::DestroySubChunk(int y)
{
	subChunk& sbc = GetSubChunk(y);

	DestroySubChunk(sbc);
}

void Chunk::DestroySubChunk(subChunk& c)
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			if (c.blocks[x][z] != nullptr)
				delete c.blocks[x][z];

			c.blocks[x][z] = nullptr;
		}
	}
}

void Chunk::DestroySubChunks()
{
	for (int i = 0; i < subChunks.size(); i++)
		DestroySubChunk(subChunks[i].y);

	subChunks.clear();
}

void Chunk::CreateSubChunks()
{
	for (int y = CHUNK_HEIGHT - 1; y > -1; y--)
	{
		subChunk sbc = CreateSubChunk(y);
		if (sbc.y > -1)
			subChunks.push_back(sbc);
	}
}

void Chunk::SetBuffer()
{
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VVertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)offsetof(VVertex, uv));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void Chunk::Init()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void Chunk::Destroy()
{
	vertices.clear();
	indices.clear();

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Chunk::Unload()
{
	DestroySubChunks();
	Destroy();
}

void Chunk::Draw()
{
	glBindVertexArray(VAO);

	// cull faces (default ccw)
	glEnable(GL_CULL_FACE);

	// bind texture pack
	txp->Bind();

	Shader* s = Game::instance->shader;
	
	// bind default shader
	s->Bind();

	// draw indices
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// unbind and clean up everything
	s->Unbind();

	txp->Unbind();

	glDisable(GL_CULL_FACE);

	glBindVertexArray(0);
}