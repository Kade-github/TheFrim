#include "Chunk.h"
#include "../../../Engine/Game.h"

void Chunk::AddToDraw(std::vector<Vertex> _v, std::vector<unsigned int> _i)
{
	vertices.insert(vertices.end(), _v.begin(), _v.end());

	for (int i = 0; i < _i.size(); i++)
		indices.push_back(_i[i] + vertices.size() - _v.size());
}

Chunk::Chunk(glm::vec3 pos, Texture* _spr) : GameObject(pos)
{
	// fill blocks with nullptr

	for (int x = 0; x < 16; x++)
	{
		for (int y = 0; y < 256; y++)
		{
			for (int z = 0; z < 16; z++)
			{
				blocks[x][y][z] = nullptr;
			}
		}
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	sheet = _spr;
}

void Chunk::AddBlock(Block* block)
{
	glm::vec3 pos = block->position;

	block->textureHeight = sheet->height;
	block->textureWidth = sheet->width;

	blocks[(int)pos.x][(int)-pos.y][(int)pos.z] = block;
}

void Chunk::GenerateMesh()
{

	vertices.clear();
	indices.clear();
	
	for (int x = position.x; x < position.x + 16; x++)
	{
		for (int y = position.y; y < position.y + 256; y++)
		{
			for (int z = position.z; z < position.z + 16; z++)
			{
				if (blocks[x][y][z] != nullptr)
				{
					Block* b = blocks[x][y][z];

					// get adjacent blocks

					Block* front = nullptr;
					Block* back = nullptr;
					Block* right = nullptr;
					Block* left = nullptr;
					Block* top = nullptr;
					Block* bottom = nullptr;

					if (z - 1 >= 0)
						front = blocks[x][y][z - 1];

					if (z + 1 < 16)
						back = blocks[x][y][z + 1];

					if (x - 1 >= 0)
						right = blocks[x - 1][y][z];

					if (x + 1 < 16)
						left = blocks[x + 1][y][z];

					if (y - 1 >= 0)
						top = blocks[x][y + 1][z];

					if (y + 1 < 256)
						bottom = blocks[x][y - 1][z];

					if (front == nullptr)
					{
						BlockFace frontFace = b->CreateFrontFace();
						AddToDraw(frontFace.vertices, frontFace.indices);
					}

					if (back == nullptr)
					{
						BlockFace backFace = b->CreateBackFace();
						AddToDraw(backFace.vertices, backFace.indices);
					}
					if (right == nullptr)
					{
						BlockFace rightFace = b->CreateRightFace();
						AddToDraw(rightFace.vertices, rightFace.indices);
					}

					if (left == nullptr)
					{
						BlockFace leftFace = b->CreateLeftFace();
						AddToDraw(leftFace.vertices, leftFace.indices);
					}

					if (top == nullptr)
					{
						BlockFace topFace = b->CreateTopFace();
						AddToDraw(topFace.vertices, topFace.indices);
					}

					if (bottom == nullptr)
					{
						BlockFace bottomFace = b->CreateBottomFace();
						AddToDraw(bottomFace.vertices, bottomFace.indices);
					}
				}
			}
		}
	}

	if (vertices.size() == 0 || indices.size() == 0)
		return;

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

}

void Chunk::Create()
{
}

void Chunk::Draw()
{
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
