#include "../../WorldManager.h"

#include <Game.h>

#include "Blocks/Grass.h"
#include "Blocks/Dirt.h"
#include "Blocks/Stone.h"


Block* subChunk::getBlock(int x, int z)
{
	return blocks[x][z];
}

int Chunk::GetBlock(float x, float y, float z)
{
	int _x = x;
	int _z = z;

	int diffX = x - position.x;
	int diffZ = z - position.z;

	if (diffX < 0)
		return 0;

	if (diffX >= CHUNK_SIZE)
		return 0;

	if (diffZ < 0)
		return 0;

	if (diffZ >= CHUNK_SIZE)
		return 0;

	if (x < 0)
		_x = std::abs(x - position.x);
	if (x >= CHUNK_SIZE)
		_x = std::abs(x - position.x);

	if (z < 0)
		_z = std::abs(z - position.z);

	if (z >= CHUNK_SIZE)
		_z = std::abs(z - position.z);

	if (_x > CHUNK_SIZE - 1)
		return 0;

	if (_z > CHUNK_SIZE - 1)
		return 0;

	if (_x < 0)
		return 0;

	if (_z < 0)
		return 0;

	if (y < 0 || y > CHUNK_HEIGHT - 1)
		return 0;

	return myData.blocks[_x][_z][(int)y];
}

Block* Chunk::GetSubBlock(int x, int y, int z)
{
	subChunk& sbc = GetSubChunk(y);

	if (sbc.y <= -1)
		return nullptr;

	int _x = x;
	int _z = z;

	if (x < 0)
		_x = std::abs(x - position.x);
	if (x >= CHUNK_SIZE)
		_x = std::abs(x - position.x);

	if (z < 0)
		_z = std::abs(z - position.z);

	if (z >= CHUNK_SIZE)
		_z = std::abs(z - position.z);

	if (_x > CHUNK_SIZE - 1)
		return nullptr;

	if (_z > CHUNK_SIZE - 1)
		return nullptr;

	if (_x < 0)
		return nullptr;

	if (_z < 0)
		return nullptr;

	if (y < 0 || y > CHUNK_HEIGHT - 1)
		return nullptr;

	return sbc.getBlock(_x, _z);
}

bool Chunk::DoesBlockExist(float x, float y, float z)
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
	bool top = true;
	bool bottom = true;
	bool left = true;
	bool right = true;
	bool front = true;
	bool back = true;

	int x = b->position.x;
	int rX = std::abs(x - position.x);
	int y = b->position.y;
	int z = b->position.z;
	int rZ = std::abs(z - position.z);

	// in our chunk
	if (DoesBlockExist(x, y + 1, z))
		top = false;

	if (DoesBlockExist(x, y - 1, z))
		bottom = false;

	if (DoesBlockExist(x + 1, y, z))
		left = false;

	if (DoesBlockExist(x - 1, y, z))
		right = false;

	if (DoesBlockExist(x, y, z - 1))
		front = false;

	if (DoesBlockExist(x, y, z + 1))
		back = false;

	// in adjacent chunks

	if (rX == CHUNK_SIZE - 1)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x - CHUNK_SIZE, position.z);
		if (c != nullptr)
		{
			if (c->DoesBlockExist(0, y, z))
				left = false;
		}
	}

	if (rX == 0)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x + CHUNK_SIZE, position.z);
		if (c != nullptr)
		{
			if (c->DoesBlockExist(CHUNK_SIZE - 1, y, z))
				right = false;
		}
	}

	if (rZ == 0)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z - CHUNK_SIZE);
		if (c != nullptr)
		{
			if (c->DoesBlockExist(x, y, CHUNK_SIZE - 1))
				front = false;
		}
	}

	if (rZ == CHUNK_SIZE - 1)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z + CHUNK_SIZE);
		if (c != nullptr)
		{
			if (c->DoesBlockExist(x, y, 0))
				back = false;
		}
	}

	// create faces

	b->faces.clear();

	if (top)
	{
		BlockFace f = b->CreateTopFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
		b->faces.push_back(f);
	}

	if (bottom)
	{
		BlockFace f = b->CreateBottomFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
		b->faces.push_back(f);
	}

	if (left)
	{
		BlockFace f = b->CreateLeftFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
		b->faces.push_back(f);
	}

	if (right)
	{
		BlockFace f = b->CreateRightFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
		b->faces.push_back(f);
	}

	if (front)
	{
		BlockFace f = b->CreateFrontFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
		b->faces.push_back(f);
	}

	if (back)
	{
		BlockFace f = b->CreateBackFace();
		vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
		for (int i = 0; i < f.indices.size(); i++)
			indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());

		b->faces.push_back(f);
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

bool Chunk::IsInChunk(float x, float z)
{
	if (x < position.x || x > position.x + CHUNK_SIZE)
		return false;

	if (z < position.z || z > position.z + CHUNK_SIZE)
		return false;

	return true;
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
	case 2:
		block = new Grass(position + glm::vec3(x, y, z));
		break;
	case 3:
		block = new Stone(position + glm::vec3(x, y, z));
		break;
	default:
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
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GameObject::VVertex), vertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));
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

	glm::mat4 model = glm::mat4(1.0f);

	s->SetUniformMat4f("model", &model[0][0]);

	// draw indices
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// unbind and clean up everything
	s->Unbind();

	txp->Unbind();

	glDisable(GL_CULL_FACE);

	glBindVertexArray(0);
}