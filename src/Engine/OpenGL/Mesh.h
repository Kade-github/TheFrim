#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Texture.h"
#include "../Objects/Camera.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 texCoords;

	// equals operator

	bool operator==(const Vertex& other) const
	{
		return position == other.position
			&& normal == other.normal
			&& texCoords == other.texCoords;
	}
};

class Mesh {
	// render data 
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh()
	{
		// create buffers/arrays
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		// load data into vertex buffers
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		// A great thing about structs is that their memory layout is sequential for all its items.
		// The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
		// again translates to 3/2 floats which translates to a byte array.
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords));

		glBindVertexArray(0);
	}

public:
    // mesh data
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*>      textures;

	unsigned int VAO;

	glm::vec3 position;

	glm::vec3 axis;
	float angle;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);

	// render the mesh
	void Draw(unsigned int shaderId);
};

#endif