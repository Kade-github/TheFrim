#include "FallingBlock.h"
#include <Game.h>
#include "../../Scenes/Gameplay.h"

FallingBlock::FallingBlock(glm::vec3 _pos, Texture* t, std::vector<GameObject::VVertex> v, std::vector<unsigned int> i, int _blockType) : Entity(_pos)
{
	txp = t;
	vertices = v;
	indices = i;
	blockType = _blockType;

	isCreature = false;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GameObject::VVertex), &vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, normal));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);
}

void FallingBlock::Draw()
{
	Shader* s = Game::instance->shader;

	glEnable(GL_DEPTH_CLAMP);

	glDisable(GL_CULL_FACE);

	glBindVertexArray(VAO); // transparent faces
	txp->Bind();
	s->Bind();

	glm::mat4 model = glm::mat4(1.0f);

	model = glm::translate(model, position);
	model = glm::rotate(model, glm::radians(angle), rotateAxis);

	s->SetUniformMat4f("model", &model[0][0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	txp->Unbind();
	s->Unbind();

	glBindVertexArray(0);

	Entity::Draw();

	if (isOnGround && !delayed)
	{
		// place block
		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		Chunk* c = gp->wm->GetChunk(position.x, position.z);

		c->ModifyBlock(position.x, position.y, position.z, blockType);
		gp->DelayedRemoveObject(this);
	}
}
