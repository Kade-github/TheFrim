#include "../../LightingManager.h"
#include "../../WorldManager.h"

#include <Game.h>

#include "Blocks/Grass.h"
#include "Blocks/Dirt.h"
#include "Blocks/Stone.h"
#include "Blocks/Cobblestone.h"

void Chunk::ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal)
{
	for(GameObject::VVertex& v : vertices)
		v.normal = normal;
}

Block* subChunk::getBlock(int x, int z)
{
	return blocks[x][z];
}

int Chunk::GetBlock(float x, float y, float z)
{
	int _x = x;
	int _z = z;

	if (x < position.x)
		return 0;

	if (x > position.x + CHUNK_SIZE)
		return 0;

	if (z < position.z)
		return 0;

	if (z > position.z + CHUNK_SIZE)
		return 0;

	glm::vec3 w = WorldToChunk(glm::vec3(x, y, z));

	_x = w.x;
	_z = w.z;

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


bool Chunk::InterchunkDoesBlockExist(float x, float y, float z)
{
	int _x = x;
	int _z = z;

	if (_x < 0)
	{
		// right chunk

		Chunk* c = WorldManager::instance->GetChunk(position.x - CHUNK_SIZE, position.z);

		if (c != nullptr)
			return c->GetBlockNoCheck(c->position.x + CHUNK_SIZE - 1, y, z) > 0;
		else
			return true;
	}

	if (_x >= CHUNK_SIZE)
	{
		// left chunk

		Chunk* c = WorldManager::instance->GetChunk(position.x + CHUNK_SIZE, position.z);

		if (c != nullptr)
			return c->GetBlockNoCheck(c->position.x, y, z) > 0;
		else
			return true;
	
	}

	if (_z < 0)
	{
		// front chunk

		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z - CHUNK_SIZE);

		if (c != nullptr)
			return c->GetBlockNoCheck(x, y, c->position.z + CHUNK_SIZE - 1) > 0;
		else
			return true;
	}

	if (_z >= CHUNK_SIZE)
	{
		// back chunk

		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z + CHUNK_SIZE);

		if (c != nullptr)
			return c->GetBlockNoCheck(x, y, c->position.z) > 0;
		else
			return true;
	}

	return GetBlockNoCheck(x, y, z) > 0;
}

int Chunk::GetBlockNoCheck(float x, float y, float z)
{
	int _x = x;
	int _z = z;
	glm::vec3 w = WorldToChunk(glm::vec3(x, y, z));

	if (_x < CHUNK_SIZE && _x >= 0)
		w.x = _x;

	if (_z < CHUNK_SIZE && _z >= 0)
		w.z = _z;

	_x = w.x;
	_z = w.z;

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

bool Chunk::DoesBlockExist(float x, float y, float z)
{
	return GetBlock(x, y, z) > 0; // if its anything under 0 (thats impossible, uint etc), and if its 0 it's air.
}

void Chunk::ModifyBlock(float x, float y, float z, int id)
{
	glm::vec3 w = WorldToChunk(glm::vec3(x, y, z));

	if (x >= 0 && x < CHUNK_SIZE - 1)
		w.x = x;

	if (z >= 0 && z < CHUNK_SIZE - 1)
		w.z = z;

	if (w.x > CHUNK_SIZE - 1)
		return;

	if (w.z > CHUNK_SIZE - 1)
		return;

	subChunk& sbc = GetSubChunk(y);

	if (sbc.y <= -1 && id > 0) // cant create air
	{
		// create subchunk

		myData.blocks[(int)w.x][(int)w.z][(int)w.y] = id;

		sbc = CreateSubChunk(y);

		if (sbc.y > -1)
			subChunks.push_back(sbc);
	}
	else
	{
		if (sbc.blocks[(int)w.x][(int)w.z] != nullptr) // if it exists, delete it if necessary
		{
			if (id <= 0)
				delete sbc.blocks[(int)w.x][(int)w.z];
		}

		if (id <= 0) // destroyed block
		{
			sbc.blocks[(int)w.x][(int)w.z] = nullptr;
			myData.blocks[(int)w.x][(int)w.z][(int)w.y] = 0;
		}
		else
		{
			sbc.blocks[(int)w.x][(int)w.z] = CreateBlock(w.x, w.y, w.z, id);
			sbc.blocks[(int)w.x][(int)w.z]->textureHeight = txp->height;
			sbc.blocks[(int)w.x][(int)w.z]->textureWidth = txp->width;

			myData.blocks[(int)w.x][(int)w.z][(int)w.y] = id;
		}
	}

	subChunk sbcBelow = GetSubChunk(y - 1);

	if (sbcBelow.y <= -1) // create below
	{
		sbcBelow = CreateSubChunk(y - 1);

		if (sbcBelow.y > -1)
			subChunks.push_back(sbcBelow);
	}

	subChunk sbcAbove = GetSubChunk(y + 1);

	if (sbcAbove.y <= -1) // create above
	{
		sbcAbove = CreateSubChunk(y + 1);

		if (sbcAbove.y > -1)
			subChunks.push_back(sbcAbove);
	}

	// check if we need to update other chunks

	RenderSubChunks();
	SetBuffer();

	if (w.x == 0)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x + CHUNK_SIZE, position.z);

		if (c != nullptr)
		{
			// check if we need to create a subchunk

			subChunk s = c->GetSubChunk(y);

			if (s.y <= -1)
			{
				s = c->CreateSubChunk(y);

				if (s.y > -1)
					c->subChunks.push_back(s);
			}


			c->RenderSubChunks();
			c->SetBuffer();
		}
	}

	if (w.x == CHUNK_SIZE - 1)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x - CHUNK_SIZE, position.z);

		if (c != nullptr)
		{
			subChunk s = c->GetSubChunk(y);

			if (s.y <= -1)
			{
				s = c->CreateSubChunk(y);

				if (s.y > -1)
					c->subChunks.push_back(s);
			}

			c->RenderSubChunks();
			c->SetBuffer();
		}
	}

	if (w.z == 0)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z + CHUNK_SIZE);

		if (c != nullptr)
		{
			subChunk s = c->GetSubChunk(y);

			if (s.y <= -1)
			{
				s = c->CreateSubChunk(y);

				if (s.y > -1)
					c->subChunks.push_back(s);
			}

			c->RenderSubChunks();
			c->SetBuffer();
		}
	}

	if (w.z == CHUNK_SIZE - 1)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z - CHUNK_SIZE);

		if (c != nullptr)
		{
			subChunk s = c->GetSubChunk(y);

			if (s.y <= -1)
			{
				s = c->CreateSubChunk(y);

				if (s.y > -1)
					c->subChunks.push_back(s);
			}

			c->RenderSubChunks();
			c->SetBuffer();
		}
	}

}

// this is made confusingly. I'm sorry.
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
	int y = b->position.y;
	int z = b->position.z;

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

	// create faces

	b->faces.clear();

	if (top)
	{
		BlockFace f = b->CreateTopFace();
		ApplyNormal(f.vertices, glm::vec3(0, 1.0f, 0));
		b->faces.push_back(f);
	}

	if (bottom)
	{
		BlockFace f = b->CreateBottomFace();

		ApplyNormal(f.vertices, glm::vec3(0, -1.0f, 0));
		b->faces.push_back(f);
	}

	if (left)
	{
		BlockFace f = b->CreateLeftFace();
		ApplyNormal(f.vertices, glm::vec3(1.0f, 0, 0));
		b->faces.push_back(f);
	}

	if (right)
	{
		BlockFace f = b->CreateRightFace();
		ApplyNormal(f.vertices, glm::vec3(-1.0f, 0, 0));
		b->faces.push_back(f);
	}

	if (front)
	{
		BlockFace f = b->CreateFrontFace();
		ApplyNormal(f.vertices, glm::vec3(0, 0, -1.0f));
		b->faces.push_back(f);
	}

	if (back)
	{
		BlockFace f = b->CreateBackFace();
		ApplyNormal(f.vertices, glm::vec3(0, 0, 1.0f));
		b->faces.push_back(f);
	}
}

void Chunk::RenderSubChunk(subChunk& sbc)
{
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

			for (BlockFace f : block->faces)
			{
				vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
				for (int i = 0; i < f.indices.size(); i++)
					indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
			}
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

		RenderSubChunk(sbc);
	}
}

glm::vec3 Chunk::WorldToChunk(glm::vec3 pos)
{
	return glm::vec3((int)std::abs(pos.x - position.x), pos.y, (int)std::abs(pos.z - position.z));
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
	empty.y = -1;
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


void Chunk::RenderSubChunkShadow(subChunk& sbc)
{
	if (sbc.y <= -1)
		return;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			Block* block = sbc.getBlock(x, z);
			if (block == nullptr)
				continue;

			for (BlockFace f : block->faces)
			{
				// check light

				int light = LightingManager::GetInstance()->GetLightLevel(block->position + f.vertices[0].normal);

				if (light < 10)
				{
					// shadow overlay
					glm::vec4 shadowUV = glm::vec4();
					if (light > 7)
						shadowUV = block->GetUVVerticallyFlipped(BUV_SHADOWTWENTYFIVE);
					else if (light > 5)
						shadowUV = block->GetUVVerticallyFlipped(BUV_SHADOWFIFTY);
					else if (light > 3)
						shadowUV = block->GetUVVerticallyFlipped(BUV_SHADOWSEVENTYFIVE);
					else
						shadowUV = block->GetUVVerticallyFlipped(BUV_SHADOWFULL);

					std::vector<GameObject::VVertex> sV = f.vertices;

					// extrude shadow
					for (GameObject::VVertex& v : sV)
						v.position += sV[0].normal * 0.005f;

					sV[0].uv = shadowUV;
					sV[1].uv = glm::vec2(shadowUV.x + shadowUV.z, shadowUV.y);
					sV[2].uv = glm::vec2(shadowUV.x, shadowUV.y + shadowUV.w);
					sV[3].uv = glm::vec2(shadowUV.x + shadowUV.z, shadowUV.y + shadowUV.w);

					shadowVertices.insert(shadowVertices.end(), sV.begin(), sV.end());
					for (int i = 0; i < f.indices.size(); i++)
						shadowIndices.push_back(f.indices[i] + shadowVertices.size() - sV.size());
				}
			}
		}
	}
}

void Chunk::RenderSubChunksShadow()
{
	shadowVertices.clear();
	shadowIndices.clear();

	for (int i = 0; i < subChunks.size(); i++)
	{
		subChunk& sbc = subChunks[i];
		if (sbc.y <= -1)
			continue;

		RenderSubChunkShadow(sbc);
	}
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
				// normally an if-else chain like this is looked down upon,
				// but honestly what would you do huh? exactly :(

				if (!DoesBlockExist(position.x + x, y + 1, position.z + z))
					isOccluded = false;
				else if (!DoesBlockExist(position.x + x, y - 1, position.z + z))
					isOccluded = false;
				else if (!InterchunkDoesBlockExist(x - 1, y, z))
					isOccluded = false;
				else if (!InterchunkDoesBlockExist(x + 1, y, z))
					isOccluded = false;
				else if (!InterchunkDoesBlockExist(x, y, z - 1))
					isOccluded = false;
				else if (!InterchunkDoesBlockExist(x, y, z + 1))
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
	case 4:
		block = new Cobblestone(position + glm::vec3(x, y, z));
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
	// bind first vertex array
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

	size = indices.size();

}

void Chunk::SetShadowBuffer()
{
	// bind shadow vertex array
	glBindVertexArray(SHADOWVAO);

	glBindBuffer(GL_ARRAY_BUFFER, SHADOWVBO);
	glBufferData(GL_ARRAY_BUFFER, shadowVertices.size() * sizeof(GameObject::VVertex), shadowVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SHADOWEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shadowIndices.size() * sizeof(unsigned int), shadowIndices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	shadowSize = shadowIndices.size();
}

void Chunk::Init()
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &SHADOWVAO);
	glGenBuffers(1, &SHADOWVBO);
	glGenBuffers(1, &SHADOWEBO);
}

void Chunk::Destroy()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteVertexArrays(1, &SHADOWVAO);
	glDeleteBuffers(1, &SHADOWVBO);
	glDeleteBuffers(1, &SHADOWEBO);
}

void Chunk::Unload()
{
	DestroySubChunks();
}

void Chunk::Draw()
{
	if (!isRendered)
		return;

	Shader* s = Game::instance->shader;

	glBindVertexArray(VAO); // regular faces
		txp->Bind();
		s->Bind();

		glm::mat4 model = glm::mat4(1.0f);

		s->SetUniformMat4f("model", &model[0][0]);

		glEnable(GL_CULL_FACE);
		glEnable(GL_DEPTH_CLAMP);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

		txp->Unbind();
		s->Unbind();
		glDisable(GL_CULL_FACE);
		glDisable(GL_DEPTH_CLAMP);
	glBindVertexArray(SHADOWVAO); // shadow faces
		glEnable(GL_CULL_FACE);
		txp->Bind();
		s->Bind();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SHADOWEBO);

		glDrawElements(GL_TRIANGLES, shadowSize, GL_UNSIGNED_INT, 0);

		s->Unbind();
		txp->Unbind();
	
		glDisable(GL_CULL_FACE);
	glBindVertexArray(0);
}