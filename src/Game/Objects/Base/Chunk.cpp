#include "Chunk.h"
#include <Game.h>
#include "Blocks/Dirt.h"
#include "Blocks/Grass.h"


Chunk::Chunk(glm::vec3 pos, Texture* _spr) : GameObject(pos)
{
	generatedVAO = false;
	sheet = _spr;
}

void Chunk::UpdateBlockFace(Block* b)
{
	// create faces

	bool front = false;
	bool back = false;
	bool left = false;
	bool right = false;
	bool top = false;
	bool bottom = false;

	// check blocks around

	front = DoesBlockExist(b->position.x, b->position.y, b->position.z - 1);
	back = DoesBlockExist(b->position.x, b->position.y, b->position.z + 1);
	left = DoesBlockExist(b->position.x + 1, b->position.y, b->position.z);
	right = DoesBlockExist(b->position.x - 1, b->position.y, b->position.z);
	top = DoesBlockExist(b->position.x, b->position.y + 1, b->position.z);
	bottom = DoesBlockExist(b->position.x, b->position.y - 1, b->position.z);

	// check blocks in other chunks

	if (frontChunk != NULL && b->position.z == 15)
		front = frontChunk->DoesBlockExist(b->position.x, b->position.y, 0);

	if (backChunk != NULL && b->position.z == 0)
		back = backChunk->DoesBlockExist(b->position.x, b->position.y, 15);

	if (leftChunk != NULL && b->position.x == 15)
		left = leftChunk->DoesBlockExist(0, b->position.y, b->position.z);

	if (rightChunk != NULL && b->position.x == 0)
		right = rightChunk->DoesBlockExist(15, b->position.y, b->position.z);

	// set texture width and height

	b->textureHeight = sheet->height;
	b->textureWidth = sheet->width;

	// create faces

	if (!front)
		b->faces.push_back(b->CreateFrontFace());

	if (!back)
		b->faces.push_back(b->CreateBackFace());

	if (!left)
		b->faces.push_back(b->CreateLeftFace());

	if (!right)
		b->faces.push_back(b->CreateRightFace());

	if (!top)
		b->faces.push_back(b->CreateTopFace());

	if (!bottom)
		b->faces.push_back(b->CreateBottomFace());
}

void Chunk::UpdateBlocks(Data::Chunk data)
{
	this->data = data;

	DeloadBlocks();

	CleanBlocks();

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			for (int y = 0; y < 256; y++)
			{
				int type = data.blocks[x][z][y];

				if (type <= 0)
					continue;

				Block* b = CreateBlock(type, glm::vec3(x, y, z));

				if (b == NULL)
					continue;

				UpdateBlockFace(b);

				if (b->faces.size() != 0)
					blocks[x][z][y] = b;
				else
					delete b;
			}
		}
	}

	isLoaded = true;
}

Block* Chunk::CreateBlock(int type, glm::vec3 _pos)
{
	Block* b = NULL;
	switch (type)
	{
	default:
		b = new Dirt(position + _pos);
		break;
	case 2:
		b = new Grass(position + _pos);
		break;
	}

	return b;
}

bool Chunk::DoesBlockExist(int x, int y, int z)
{
	int _x = x;
	int _z = z;

	if (_x < 0 || _x > 15)
		_x = x - position.x;

	if (_z < 0 || _z > 15)
		_z = z - position.z;
	
	if (_z < 0 || _z > 15 || _x < 0 || _x > 15 || y < 0 || y > 255)
		return false;

	return data.blocks[_x][_z][y] != NULL;
}

Block* Chunk::GetBlock(float x, float y, float z)
{
	float __x = x;
	float __z = z;

	if (__x < 0 || __x > 15)
		__x = x - position.x;

	if (__z < 0 || __z > 15)
		__z = z - position.z;

	int _x = __x;
	int _z = __z;

	if (_z < 0 || _z > 15 || _x < 0 || _x > 15 || y < 0 || y > 255)
		return nullptr;

	return blocks[_x][_z][(int)y];
}

void Chunk::DeloadBlocks()
{
	if (!isLoaded)
		return;

	if (rendered)
		DeloadMesh();

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			for (int y = 0; y < 256; y++)
			{
				Block* b = blocks[x][z][y];

				if (b != NULL)
					delete b;

				blocks[x][z][y] = NULL;
			}
		}
	}

	isLoaded = false;
}

void Chunk::RemoveBlock(int x, int y, int z)
{
	Block* b = GetBlock(x, y, z);

	if (b == NULL)
		return;

	blocks[(int)x][(int)z][(int)y] = NULL;

	delete b;

	// get blocks around it

	Block* front = GetBlock(x, y, z - 1);
	Block* back = GetBlock(x, y, z + 1);
	Block* left = GetBlock(x - 1, y, z);
	Block* right = GetBlock(x + 1, y, z);
	Block* top = GetBlock(x, y + 1, z);
	Block* bottom = GetBlock(x, y - 1, z);

	// update faces

	if (front != NULL)
		UpdateBlockFace(front);

	if (back != NULL)
		UpdateBlockFace(back);

	if (left != NULL)
		UpdateBlockFace(left);

	if (right != NULL)
		UpdateBlockFace(right);

	if (top != NULL)
		UpdateBlockFace(top);

	if (bottom != NULL)
		UpdateBlockFace(bottom);

	// update faces in other chunks

	if (frontChunk != NULL && z == 0)
	{
		Block* f = frontChunk->GetBlock(x, y, 15);

		if (f != NULL)
		{
			frontChunk->UpdateBlockFace(f);
			frontChunk->DeloadMesh();
			frontChunk->GenerateMesh();
		}
	}

	if (backChunk != NULL && z == 15)
	{
		Block* f = backChunk->GetBlock(x, y, 0);

		if (f != NULL)
		{
			backChunk->UpdateBlockFace(f);
			backChunk->DeloadMesh();
			backChunk->GenerateMesh();
		}
	}

	if (leftChunk != NULL && x == 0)
	{
		Block* f = leftChunk->GetBlock(15, y, z);

		if (f != NULL)
		{
			leftChunk->UpdateBlockFace(f);
			leftChunk->DeloadMesh();
			leftChunk->GenerateMesh();
		}
	}

	if (rightChunk != NULL && x == 15)
	{
		Block* f = rightChunk->GetBlock(0, y, z);

		if (f != NULL)
		{
			rightChunk->UpdateBlockFace(f);
			rightChunk->DeloadMesh();
			rightChunk->GenerateMesh();
		}
	}

	// update mesh

	DeloadMesh();
	GenerateMesh();
}

void Chunk::PlaceBlock(int x, int y, int z, int type)
{
	if (DoesBlockExist(x, y, z))
		return;

	Block* b = CreateBlock(type, glm::vec3(x, y, z));

	if (b == NULL)
		return;

	UpdateBlockFace(b);

	if (b->faces.size() != 0)
		blocks[x][z][y] = b;
	else
		delete b;

	// get blocks around it

	Block* front = GetBlock(x, y, z - 1);
	Block* back = GetBlock(x, y, z + 1);
	Block* left = GetBlock(x - 1, y, z);
	Block* right = GetBlock(x + 1, y, z);
	Block* top = GetBlock(x, y + 1, z);
	Block* bottom = GetBlock(x, y - 1, z);

	// update faces

	if (front != NULL)
		UpdateBlockFace(front);

	if (back != NULL)
		UpdateBlockFace(back);

	if (left != NULL)
		UpdateBlockFace(left);

	if (right != NULL)
		UpdateBlockFace(right);

	if (top != NULL)
		UpdateBlockFace(top);

	if (bottom != NULL)
		UpdateBlockFace(bottom);

	// update faces in other chunks

	if (frontChunk != NULL && z == 0)
	{
		Block* f = frontChunk->GetBlock(x, y, 15);

		if (f != NULL)
		{
			frontChunk->UpdateBlockFace(f);
			frontChunk->DeloadMesh();
			frontChunk->GenerateMesh();
		}
	}

	if (backChunk != NULL && z == 15)
	{
		Block* f = backChunk->GetBlock(x, y, 0);

		if (f != NULL)
		{
			backChunk->UpdateBlockFace(f);
			backChunk->DeloadMesh();
			backChunk->GenerateMesh();
		}
	}

	if (leftChunk != NULL && x == 0)
	{
		Block* f = leftChunk->GetBlock(15, y, z);

		if (f != NULL)
		{
			leftChunk->UpdateBlockFace(f);
			leftChunk->DeloadMesh();
			leftChunk->GenerateMesh();
		}
	}

	if (rightChunk != NULL && x == 15)
	{
		Block* f = rightChunk->GetBlock(0, y, z);

		if (f != NULL)
		{
			rightChunk->UpdateBlockFace(f);
			rightChunk->DeloadMesh();
			rightChunk->GenerateMesh();
		}
	}

	// update mesh

	DeloadMesh();
	GenerateMesh();
}

void Chunk::CleanBlocks()
{
	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			for (int y = 0; y < 256; y++)
			{
				blocks[x][z][y] = NULL;
			}
		}
	}
}

void Chunk::GenerateMesh()
{
	vertices.clear();
	indices.clear();

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			for (int y = 0; y < 256; y++)
			{
				Block* b = blocks[x][z][y];

				if (b == NULL)
					continue;

				b->Draw(vertices, indices);
			}
		}
	}

	if (!generatedVAO)
	{
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		generatedVAO = true;
	}

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

	rendered = true;
}

void Chunk::DeloadMesh()
{
	if (!rendered)
		return;

	rendered = false;

	if (generatedVAO)
	{
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);

		generatedVAO = false;
	}
}

void Chunk::Create()
{

}

void Chunk::Draw()
{
	if (!rendered)
		return;


	glBindVertexArray(VAO);

	glEnable(GL_CULL_FACE);

	sheet->Bind();

	Shader* s = Game::instance->shader;
	Camera* c = Game::instance->GetCamera();

	s->Bind();
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	s->Unbind();

	sheet->Unbind();

	glDisable(GL_CULL_FACE);

	glBindVertexArray(0);
}

Block* Chunk::GetTopBlock(int x, int z)
{
	for (int y = 255; y > 0; y--)
	{
		Block* b = GetBlock(x, y, z);

		if (b != NULL)
			return b;
	}

	return NULL;
}
