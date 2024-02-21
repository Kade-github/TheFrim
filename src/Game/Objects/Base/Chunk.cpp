#include "Chunk.h"
#include <Game.h>
#include "Blocks/Dirt.h"
#include "Blocks/Grass.h"

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

void Chunk::GenerateMesh(Data::Chunk c, Data::Chunk forwardC, Data::Chunk backwardC, Data::Chunk leftC, Data::Chunk rightC)
{
	if (isLoaded)
		return;

	blocks.clear();

	vertices.clear();
	indices.clear();

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 256; y++)
		{
			for (int z = 0; z < 16; z++)
			{
				if (c.blocks[y][x][z] >= 1)
				{
					int dB = c.blocks[y][x][z];

					// get adjacent blocks

					bool front = false;
					bool back = false;
					bool right = false;
					bool left = false;
					bool top = false;
					bool bottom = false;

					if (z - 1 >= 0)
						front = c.blocks[y][x][z - 1] >= 1;

					if (z + 1 < 16)
						back = c.blocks[y][x][z + 1] >= 1;

					if (x - 1 >= 0)
						right = c.blocks[y][x][z] >= 1;

					if (x + 1 < 16)
						left = c.blocks[y][x][z] >= 1;

					if (y - 1 >= 0)
						top = c.blocks[y - 1][x][z] >= 1;

					if (y + 1 < 256)
						bottom = c.blocks[y + 1][x][z] >= 1;

					if (z == 0)
						front = forwardC.blocks[y][x][15] >= 1;

					if (z == 15)
						back = backwardC.blocks[y][x][0] >= 1;

					if (x == 0)
						left = leftC.blocks[y][15][z] >= 1;

					if (x == 15)
						right = rightC.blocks[y][0][z] >= 1;

					Block* b = nullptr;

					int realY = 128 + (128 - y);

					switch (dB)
					{
					case 1:
						b = new Dirt(position + glm::vec3(x, realY, z));
						blocks.push_back(b);
						break;
					case 2:
						b = new Grass(position + glm::vec3(x, realY, z));
						blocks.push_back(b);
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

	for (auto b : blocks)
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

	s->SetUniformMat4f("view", &c->GetViewMatrix()[0][0]);
	s->SetUniformMat4f("projection", &c->GetProjectionMatrix()[0][0]);

	glm::mat4 model = glm::mat4(1.0f);

	s->SetUniformMat4f("model", &model[0][0]);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	Game::instance->shader->Bind();

	sheet->Unbind();

	glDisable(GL_CULL_FACE);

	glBindVertexArray(0);
}
