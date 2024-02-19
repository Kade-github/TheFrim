#include "Block.h"

std::vector<Vertex> Block::CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv)
{
	std::vector<Vertex> vertices = {};

	Vertex tl = Vertex(glm::vec3(position.x, position.y, position.z), glm::vec2(uv.x, uv.y));
	Vertex tr = Vertex(glm::vec3(position.x + size.x, position.y, position.z + size.z), glm::vec2(uv.x + uv.z, uv.y));
	Vertex bl = Vertex(glm::vec3(position.x, position.y + size.y, position.z + z), glm::vec2(uv.x, uv.y + uv.w));
	Vertex br = Vertex(glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z + z), glm::vec2(uv.x + uv.z, uv.y + uv.w));

	vertices.push_back(tr);
	vertices.push_back(tl);
	vertices.push_back(bl);
	vertices.push_back(br);

	return vertices;
}

void Block::CreateFaces()
{
	std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

	// front face

	std::vector<Vertex> frontVertices = CreateQuad(position, glm::vec3(1, 1, 0), 0, glm::vec4(0, 0, 0.167, 1));

	BlockFace frontFace = BlockFace(frontVertices, indices);

	frontFace.sum = frontVertices[0].position + frontVertices[1].position + frontVertices[2].position + frontVertices[3].position;

	std::swap(indices[0], indices[1]);
	std::swap(indices[3], indices[4]);

	// back face

	std::vector<Vertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, glm::vec4(0.167, 0, 0.167, 1));

	// right face

	std::vector<Vertex> rightVertices = CreateQuad(position, glm::vec3(0, 1, 1), 0, glm::vec4(0.5, 0, 0.167, 1));

	BlockFace rightFace = BlockFace(rightVertices, indices);
	BlockFace backFace = BlockFace(backVertices, indices);

	rightFace.sum = rightVertices[0].position + rightVertices[1].position + rightVertices[2].position + rightVertices[3].position;
	backFace.sum = backVertices[0].position + backVertices[1].position + backVertices[2].position + backVertices[3].position;

	std::swap(indices[0], indices[1]);
	std::swap(indices[3], indices[4]);

	// left face

	std::vector<Vertex> leftVertices = CreateQuad(position + glm::vec3(1,0,0), glm::vec3(0, 1, 1), 0, glm::vec4(0.33, 0, 0.167, 1));

	BlockFace leftFace = BlockFace(leftVertices, indices);

	leftFace.sum = leftVertices[0].position + leftVertices[1].position + leftVertices[2].position + leftVertices[3].position;


	// top face

	std::vector<Vertex> topVertices = CreateQuad(position + glm::vec3(0,1,0), glm::vec3(1, 0, 0), 1, glm::vec4(0.833, 0, 0.167, 1));

	BlockFace topFace = BlockFace(topVertices, indices);

	topFace.sum = topVertices[0].position + topVertices[1].position + topVertices[2].position + topVertices[3].position;

	// bottom face

	std::vector<Vertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, glm::vec4(0.66, 0, 0.167, 1));

	BlockFace bottomFace = BlockFace(bottomVertices, indices);

	bottomFace.sum = bottomVertices[0].position + bottomVertices[1].position + bottomVertices[2].position + bottomVertices[3].position;

	faces.push_back(topFace);
	faces.push_back(bottomFace);
	faces.push_back(leftFace);
	faces.push_back(backFace);
	faces.push_back(frontFace);
	faces.push_back(rightFace);

}

Block::Block(glm::vec3 _position, BlockType _type)
{
	position = _position;
	type = _type;

	faces = { };
}


BlockFace Block::GetFrontFace()
{
	return faces[4];
}

BlockFace Block::GetBackFace()
{
	return faces[3];
}

BlockFace Block::GetLeftFace()
{
	return faces[2];
}

BlockFace Block::GetRightFace()
{
	return faces[5];
}

BlockFace Block::GetTopFace()
{
	return faces[0];
}

BlockFace Block::GetBottomFace()
{
	return faces[1];
}
