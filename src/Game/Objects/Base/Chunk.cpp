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
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	sheet = _spr;
}

void Chunk::AddBlock(Block* block)
{
	block->CreateFaces();
	glm::vec3 pos = block->position;

	if (pos.x == -1)
		pos.x = 15;

	blocks[pos.x][pos.y][pos.z] = block;
}

void Chunk::GenerateMesh()
{
	vertices.clear();
	indices.clear();

	for (auto& x : blocks)
	{
		for (auto& y : x.second)
		{
			for (auto& z : y.second)
			{
				Block* b = z.second;

				if (b == NULL)
					continue;

				// get adjacent blocks

				Block* front = blocks[x.first][y.first][z.first - 1];
				Block* back = blocks[x.first][y.first][z.first + 1];
				Block* right = blocks[x.first - 1][y.first][z.first];
				Block* left = blocks[x.first + 1][y.first][z.first];
				Block* top = blocks[x.first][y.first + 1][z.first];
				Block* bottom = blocks[x.first][y.first - 1][z.first];

				BlockFace frontFace = b->GetFrontFace();
				BlockFace backFace = b->GetBackFace();
				BlockFace rightFace = b->GetRightFace();
				BlockFace leftFace = b->GetLeftFace();
				BlockFace topFace = b->GetTopFace();
				BlockFace bottomFace = b->GetBottomFace();

				if (front == nullptr)
					AddToDraw(frontFace.vertices, frontFace.indices);

				if (back == nullptr)
					AddToDraw(backFace.vertices, backFace.indices);

				if (right == nullptr)
					AddToDraw(rightFace.vertices, rightFace.indices);

				if (left == nullptr)
					AddToDraw(leftFace.vertices, leftFace.indices);

				if (top == nullptr)
					AddToDraw(topFace.vertices, topFace.indices);

				if (bottom == nullptr)
					AddToDraw(bottomFace.vertices, bottomFace.indices);
				
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

void Chunk::Draw()
{
	if (vertices.size() == 0 || indices.size() == 0)
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
