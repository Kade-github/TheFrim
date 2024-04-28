#ifndef _MESH_H_
#define _MESH_H_

#include <vector>
#include "Texture.h"
#include "../Objects/Camera.h"

struct Vertex
{
	glm::vec3 position;
	glm::vec2 texCoords;

	// equals operator

	bool operator==(const Vertex& other) const
	{
		return position == other.position
			&& texCoords == other.texCoords;
	}
};

class Mesh {
	// render data 
	unsigned int VBO, EBO;

	// initializes all the buffer objects/arrays
	void setupMesh();

public:
    // mesh data
	std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;
	Texture* uv;

	std::string name;

	unsigned int VAO;

	glm::vec3 position = glm::vec3(0, 0, 0);
	glm::vec3 scale = glm::vec3(1, 1, 1);
	glm::vec3 axis = glm::vec3(0, 1, 0);
	float angle = 0;

	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices);

	// render the mesh
	void Draw();
};

#endif