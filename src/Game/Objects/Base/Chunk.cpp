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

	for(int x = 0; x < 16; x++)
		for (int z = 0; z < 16; z++)
			topBlocks[x][z] = 0;

	blocks.clear();

	vertices.clear();
	indices.clear();

	for (int x = 0; x < 16; x++)
	{
		for (int z = 0; z < 16; z++)
		{
			for (int y = 255; y > 0; y--)
			{
				if (c.blocks[x][z][y] >= 1)
				{
					int dB = c.blocks[x][z][y];

					// get adjacent blocks

					bool front = false;
					bool back = false;
					bool right = false;
					bool left = false;
					bool top = false;
					bool bottom = false;

					if (z + 1 < 16 && c.blocks[x][z + 1][y] >= 1)
					{
						back = true;
					}

					if (z - 1 >= 0 && c.blocks[x][z - 1][y] >= 1)
						front = true;

					if (x + 1 < 16 && c.blocks[x + 1][z][y] >= 1)
					{
						left = true;
					}

					if (x - 1 >= 0 && c.blocks[x - 1][z][y] >= 1)
					{
						right = true;
					}

					if (y - 1 >= 0 && c.blocks[x][z][y - 1] >= 1)
						bottom = true;

					if (y + 1 < 256 && c.blocks[x][z][y + 1] >= 1)
						top = true;

					if (z == 0 && !front && forwardC.isGenerated && forwardC.blocks[x][15][y] >= 1)
					{
						front = true;
					}
					if (z == 15 && !back && backwardC.isGenerated && backwardC.blocks[x][0][y] >= 1)
					{
						back = true;
					}

					if (x == 0 && !right && rightC.isGenerated && rightC.blocks[15][z][y] >= 1)
					{
						right = true;
					}

					if (x == 15 && !left && leftC.isGenerated && leftC.blocks[0][z][y] >= 1)
					{
						left = true;
					}


					Block* b = nullptr;

					switch (dB)
					{
					default:
						b = new Dirt(position + glm::vec3(x, y, z));
						blocks.push_back(b);
						break;
					case 2:
						b = new Grass(position + glm::vec3(x, y, z));
						blocks.push_back(b);
						break;
					}

					if (b == nullptr)
						continue;

					if (topBlocks[x][z] < y)
						topBlocks[x][z] = y;

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

	s->Unbind();

	sheet->Unbind();

	glDisable(GL_CULL_FACE);

	glBindVertexArray(0);
}
