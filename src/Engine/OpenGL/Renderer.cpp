#include "Renderer.h"

void Renderer::Init()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
}

void Renderer::Draw(Texture* texture, Shader* shader, Camera* camera, glm::vec3 axis, float angle)
{
	if (vertices.size() == 0)
		return;

	if (texture != NULL)
		texture->Bind();
    shader->Bind();

	// shader lighting

	int lightPosLoc = glGetUniformLocation(shader->program, "lightPos");
	glUniform3f(lightPosLoc, camera->lightPos.x, camera->lightPos.y, camera->lightPos.z);

	int lightColorLoc = glGetUniformLocation(shader->program, "lightColor");
	glUniform3f(lightColorLoc, camera->lightColor.x, camera->lightColor.y, camera->lightColor.z);

	int projLoc = glGetUniformLocation(shader->program, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(angle), axis);

	int modelLoc = glGetUniformLocation(shader->program, "model");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	int viewLoc = glGetUniformLocation(shader->program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);

	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	shader->Unbind();

	if (texture != NULL)
		texture->Unbind();

	vertices.clear();
}
