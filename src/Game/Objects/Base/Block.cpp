#include "Block.h"

std::vector<VVertex> Block::CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv)
{
	std::vector<VVertex> vertices = {};

	VVertex tl = VVertex(glm::vec3(position.x, position.y, position.z), glm::vec2(uv.x, uv.y));
	VVertex tr = VVertex(glm::vec3(position.x + size.x, position.y, position.z + size.z), glm::vec2(uv.x + uv.z, uv.y));
	VVertex bl = VVertex(glm::vec3(position.x, position.y + size.y, position.z + z), glm::vec2(uv.x, uv.y + uv.w));
	VVertex br = VVertex(glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z + z), glm::vec2(uv.x + uv.z, uv.y + uv.w));

	vertices.push_back(tr);
	vertices.push_back(tl);
	vertices.push_back(bl);
	vertices.push_back(br);

	return vertices;
}

BlockFace Block::CreateFrontFace()
{
	std::vector<VVertex> frontVertices = CreateQuad(position, glm::vec3(1, 1, 0), 0, glm::vec4());

	return BlockFace(frontVertices, { 0, 1, 3, 1, 2, 3 });
}

BlockFace Block::CreateBackFace()
{
	std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

	std::vector<VVertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, glm::vec4());

	std::swap(indices[0], indices[1]);
	std::swap(indices[3], indices[4]);

	return BlockFace(backVertices, indices);
}

BlockFace Block::CreateLeftFace()
{
	std::vector<VVertex> leftVertices = CreateQuad(position + glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), 0, glm::vec4());

	return BlockFace(leftVertices, { 0, 1, 3, 1, 2, 3 });
}

BlockFace Block::CreateRightFace()
{
	std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

	std::vector<VVertex> rightVertices = CreateQuad(position, glm::vec3(0, 1, 1), 0, glm::vec4());

	std::swap(indices[0], indices[1]);
	std::swap(indices[3], indices[4]);

	return BlockFace(rightVertices, indices);
}

BlockFace Block::CreateTopFace()
{
	std::vector<VVertex> topVertices = CreateQuad(position + glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1, glm::vec4());

	return BlockFace(topVertices, { 0, 1, 3, 1, 2, 3 });
}

BlockFace Block::CreateBottomFace()
{
	std::vector<VVertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, glm::vec4());

	return BlockFace(bottomVertices, { 0, 1, 3, 1, 2, 3 });
}

glm::vec4 Block::GetUVVerticallyFlipped(int x, int y)
{
	float _x = x / textureWidth;
	float _y = (y / textureHeight) - 0.01f;
	float width = blockWidth / textureWidth;
	float height = blockHeight / textureHeight;

	return glm::vec4(_x, _y + height, width, -height);
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

glm::vec4 Block::GetUV(int x, int y)
{
	float _x = x / textureWidth;
	float _y = y / textureHeight;
	float width = blockWidth / textureWidth;
	float height = blockHeight / textureHeight;

	return glm::vec4(_x, _y, width, height);
	
}