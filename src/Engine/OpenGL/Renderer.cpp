#include "Renderer.h"

void Renderer::Init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void Renderer::Draw(Camera* camera)
{
	glEnable(GL_CULL_FACE);

	for (int i = 0; i < drawCalls.size(); i++)
	{
		DrawCall& c = drawCalls[i];

		Shader* shader = c.shader;
		Texture* texture = c.texture;

		shader->Bind();
		if (texture != NULL)
			texture->Bind();

		float angle = c.angle;
		glm::vec3 axis = c.axis;

		model = glm::mat4(1.0f);
		model = glm::translate(model, c.position);
		model = glm::rotate(model, glm::radians(angle), axis);

		int modelLoc = glGetUniformLocation(shader->program, "model");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

		int viewLoc = glGetUniformLocation(shader->program, "view");
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

		int projLoc = glGetUniformLocation(shader->program, "projection");
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

		for (auto& mesh : c.m->meshes)
		{
			glBindVertexArray(VAO);
			glBindBuffer(GL_ARRAY_BUFFER, VBO);
			glBufferData(GL_ARRAY_BUFFER, mesh.vertices.size() * sizeof(Vertex), &mesh.vertices[0], GL_STATIC_DRAW);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(unsigned int), &mesh.indices[0], GL_STATIC_DRAW);

			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
			glEnableVertexAttribArray(2);

			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		}

		shader->Unbind();

		if (texture != NULL)
			texture->Unbind();
	}

	drawCalls.clear();
}
