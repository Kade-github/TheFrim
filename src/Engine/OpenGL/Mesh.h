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
public:
    // mesh data
    std::vector<Vertex>       vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture*>      textures;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture*> textures);
};

#endif