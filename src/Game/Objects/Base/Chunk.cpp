#include "Chunk.h"
#include <Game.h>
#include "Blocks/Dirt.h"
#include "Blocks/Grass.h"

int Chunk::getTopBlock(float x, float z)
{
	if (x < 0 || x > 15 || z < 0 || z > 15)
		return 128;

	for (int y = 255; y > -1; y--)
	{
		int _y = doesBlockExist(x, y, z);

		if (_y >= 0)
			return _y;
	}

	return 0;
}

int Chunk::doesBlockExist(float x, float y, float z)
{
	if ((int)x < 0 || (int)x > 15 || (int)y < 0 || (int)y > 255 || (int)z < 0 || (int)z > 15)
		return -1;

	int bType = data->blocks[(int)x][(int)z][(int)y];

	if (bType >= 1)
		return (int)y;
	return -1;
}

Block* Chunk::getBlock(float x, float y, float z)
{
	for (auto& b : blocks)
	{
		if ((int)b->position.x == (int)x && (int)b->position.y == (int)y && (int)b->position.z == (int)z)
			return b;
	}
	return nullptr;
}

void Chunk::AddToDraw(std::vector<VVertex> _v, std::vector<unsigned int> _i)
{
	vertices.insert(vertices.end(), _v.begin(), _v.end());

	for (int i = 0; i < _i.size(); i++)
		indices.push_back(_i[i] + vertices.size() - _v.size());
}

Chunk::Chunk(glm::vec3 pos, Texture* _spr) : GameObject(pos)
{
	generatedVAO = false;
	sheet = _spr;
}

void Chunk::CheckAdjacent(bool& back, bool& front, bool& left, bool& right, bool& bottom, bool& top, int x, int y, int z)
{
	if (z + 1 < 16 && data->blocks[x][z + 1][y] >= 1)
		back = true;

	if (z - 1 >= 0 && data->blocks[x][z - 1][y] >= 1)
		front = true;

	if (x + 1 < 16 && data->blocks[x + 1][z][y] >= 1)
		left = true;

	if (x - 1 >= 0 && data->blocks[x - 1][z][y] >= 1)
		right = true;

	if (y - 1 >= 0 && data->blocks[x][z][y - 1] >= 1)
		bottom = true;

	if (y + 1 < 256 && data->blocks[x][z][y + 1] >= 1)
		top = true;

	if (z == 0 && !front && forwardC != nullptr && forwardC->data != NULL && forwardC->data->blocks[x][15][y] >= 1)
		front = true;
	if (z == 15 && !back && backwardC != nullptr && backwardC->data != NULL && backwardC->data->blocks[x][0][y] >= 1)
		back = true;

	if (x == 0 && !right && rightC != nullptr && rightC->data != NULL && rightC->data->blocks[15][z][y] >= 1)
		right = true;

	if (x == 15 && !left && leftC != nullptr && leftC->data != NULL && leftC->data->blocks[0][z][y] >= 1)
		left = true;
}

void Chunk::GenerateMesh(Chunk* _forward, Chunk* _backward, Chunk* _left, Chunk* _right)
{
	if (isLoaded)
		return;

	forwardC = _forward;
	backwardC = _backward;
	leftC = _left;
	rightC = _right;

	blocks.clear();

	vertices.clear();
	indices.clear();

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			for (int y = 255; y > -1; y--)
			{
				if (data->blocks[x][z][y] >= 1)
				{
					int dB = data->blocks[x][z][y];

					// get adjacent blocks

					bool front = false;
					bool back = false;
					bool right = false;
					bool left = false;
					bool top = false;
					bool bottom = false;

					CheckAdjacent(back, front, left, right, bottom, top, x, y, z);

					Block* b = nullptr;

					switch (dB)
					{
					default:
						b = new Dirt(position + glm::vec3(x, y, z));
						break;
					case 2:
						b = new Grass(position + glm::vec3(x, y, z));
						break;
					}

					if (b == nullptr)
						continue;

					b->textureHeight = sheet->height;
					b->textureWidth = sheet->width;

					if (!front)
					{
						BlockFace frontFace = b->CreateFrontFace();
						AddToDraw(frontFace.vertices, frontFace.indices);
					}

					if (!back)
					{
						BlockFace backFace = b->CreateBackFace();
						AddToDraw(backFace.vertices, backFace.indices);
					}
					if (!right)
					{
						BlockFace rightFace = b->CreateRightFace();
						AddToDraw(rightFace.vertices, rightFace.indices);
					}

					if (!left)
					{
						BlockFace leftFace = b->CreateLeftFace();
						AddToDraw(leftFace.vertices, leftFace.indices);
					}

					if (!top)
					{
						BlockFace topFace = b->CreateTopFace();
						AddToDraw(topFace.vertices, topFace.indices);
					}

					if (!bottom)
					{
						BlockFace bottomFace = b->CreateBottomFace();
						AddToDraw(bottomFace.vertices, bottomFace.indices);
					}

					blocks.push_back(b);
				}
			}
		}
	}


	isLoaded = true;
}

void Chunk::UploadMesh()
{
	if (rendered)
		return;

	if (vertices.size() == 0 || indices.size() == 0)
	{
		return;
	}

	if (!generatedVAO)
	{
		generatedVAO = true;
		glGenVertexArrays(1, &VAO);
	}

	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)offsetof(VVertex, uv));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	rendered = true;
}

void Chunk::Clean()
{
	if (!rendered)
		return;
	glBindVertexArray(VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glBindVertexArray(0);

	rendered = false;
}


void Chunk::UnloadMesh()
{
	if (!isLoaded)
		return;

	for (auto& b : blocks)
		delete b;

	blocks.clear();

	vertices.clear();
	indices.clear();

	isLoaded = false;
}

void Chunk::Create()
{
}

void Chunk::Draw()
{
	if (!rendered)
		return;

	if (indices.size() == 0)
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

void Chunk::Reload()
{
	if (!isLoaded)
		return;

	vertices.clear();
	indices.clear();

	std::vector<Block*> toDelete = {};

	bool checkForAdjChunks = false;

	for (Block* b : blocks)
	{
		int x = (int)(b->position.x - position.x);
		int y = (int)(b->position.y - position.y);
		int z = (int)(b->position.z - position.z);

		if (b->changed)
		{
			int newType = data->blocks[x][z][y];
			b->changed = false;
			if (x == 0 || z == 0 || x == 15 || z == 15)
				checkForAdjChunks = true;
			if (newType == 0)
			{
				toDelete.push_back(b);
				continue;
			}

		}

		// get adjacent blocks

		bool front = false;
		bool back = false;
		bool right = false;
		bool left = false;
		bool top = false;
		bool bottom = false;

		CheckAdjacent(back, front, left, right, bottom, top, x, y, z);

		if (!front)
		{
			BlockFace frontFace = b->CreateFrontFace();
			AddToDraw(frontFace.vertices, frontFace.indices);
		}

		if (!back)
		{
			BlockFace backFace = b->CreateBackFace();
			AddToDraw(backFace.vertices, backFace.indices);
		}

		if (!right)
		{
			BlockFace rightFace = b->CreateRightFace();
			AddToDraw(rightFace.vertices, rightFace.indices);
		}

		if (!left)
		{
			BlockFace leftFace = b->CreateLeftFace();
			AddToDraw(leftFace.vertices, leftFace.indices);
		}

		if (!top)
		{
			BlockFace topFace = b->CreateTopFace();
			AddToDraw(topFace.vertices, topFace.indices);
		}

		if (!bottom)
		{
			BlockFace bottomFace = b->CreateBottomFace();
			AddToDraw(bottomFace.vertices, bottomFace.indices);
		}
	}

	for (Block* b : toDelete)
	{
		blocks.erase(std::remove(blocks.begin(), blocks.end(), b), blocks.end());
		delete b;
	}

	if (checkForAdjChunks)
	{
		if (forwardC != nullptr && forwardC->isLoaded)
			forwardC->Reload();
		if (backwardC != nullptr && backwardC->isLoaded)
			backwardC->Reload();
		if (leftC != nullptr && leftC->isLoaded)
			leftC->Reload();
		if (rightC != nullptr && rightC->isLoaded)
			rightC->Reload();
	}

	if (rendered)
	{
		glBindVertexArray(VAO);
		glDeleteBuffers(1, &VBO);
		glDeleteBuffers(1, &EBO);
		glBindVertexArray(0);

		glBindVertexArray(VAO);

		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VVertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)0);
		glEnableVertexAttribArray(0);

		// uv attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VVertex), (void*)offsetof(VVertex, uv));
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}