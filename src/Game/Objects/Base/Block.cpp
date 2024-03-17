#include "Block.h"

std::vector<GameObject::VVertex> Block::CreateQuad(glm::vec3 position, glm::vec3 size, float z, glm::vec4 uv)
{
	std::vector<GameObject::VVertex> vertices = {};

	GameObject::VVertex tl = GameObject::VVertex(glm::vec3(position.x, position.y, position.z), glm::vec2(uv.x, uv.y));
	GameObject::VVertex tr = GameObject::VVertex(glm::vec3(position.x + size.x, position.y, position.z + size.z), glm::vec2(uv.x + uv.z, uv.y));
	GameObject::VVertex bl = GameObject::VVertex(glm::vec3(position.x, position.y + size.y, position.z + z), glm::vec2(uv.x, uv.y + uv.w));
	GameObject::VVertex br = GameObject::VVertex(glm::vec3(position.x + size.x, position.y + size.y, position.z + size.z + z), glm::vec2(uv.x + uv.z, uv.y + uv.w));

	vertices.push_back(tr);
	vertices.push_back(tl);
	vertices.push_back(bl);
	vertices.push_back(br);

	return vertices;
}

BlockFace Block::CreateFrontFace()
{
	std::vector<GameObject::VVertex> frontVertices = CreateQuad(position, glm::vec3(1, 1, 0), 0, glm::vec4());

	return BlockFace(frontVertices, { 0, 1, 3, 1, 2, 3 });
}

BlockFace Block::CreateBackFace()
{
	std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

	std::vector<GameObject::VVertex> backVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 1, 0), 0, glm::vec4());

	std::swap(indices[0], indices[1]);
	std::swap(indices[3], indices[4]);

	return BlockFace(backVertices, indices);
}

BlockFace Block::CreateLeftFace()
{
	std::vector<GameObject::VVertex> leftVertices = CreateQuad(position + glm::vec3(1, 0, 0), glm::vec3(0, 1, 1), 0, glm::vec4());

	return BlockFace(leftVertices, { 0, 1, 3, 1, 2, 3 });
}

BlockFace Block::CreateRightFace()
{
	std::vector<unsigned int> indices = { 0, 1, 3, 1, 2, 3 };

	std::vector<GameObject::VVertex> rightVertices = CreateQuad(position, glm::vec3(0, 1, 1), 0, glm::vec4());

	std::swap(indices[0], indices[1]);
	std::swap(indices[3], indices[4]);

	return BlockFace(rightVertices, indices);
}

BlockFace Block::CreateTopFace()
{
	std::vector<GameObject::VVertex> topVertices = CreateQuad(position + glm::vec3(0, 1, 0), glm::vec3(1, 0, 0), 1, glm::vec4());

	return BlockFace(topVertices, { 0, 1, 3, 1, 2, 3 });
}

BlockFace Block::CreateBottomFace()
{
	std::vector<GameObject::VVertex> bottomVertices = CreateQuad(position + glm::vec3(0, 0, 1), glm::vec3(1, 0, 0), -1, glm::vec4());

	return BlockFace(bottomVertices, { 0, 1, 3, 1, 2, 3 });
}

void Block::Draw(std::vector<GameObject::VVertex>& verts, std::vector<unsigned int>& inds)
{
	for (int i = 0; i < faces.size(); i++)
	{
		for (int j = 0; j < faces[i].vertices.size(); j++)
			verts.push_back(faces[i].vertices[j]);

		for (int j = 0; j < faces[i].indices.size(); j++)
		{
			int _i = faces[i].indices[j] + verts.size() - faces[i].vertices.size();
			inds.push_back(_i);
		}
	}
}

glm::vec4 Block::GetUVVerticallyFlipped(int x, int y)
{
	float srcW = blockWidth / textureWidth;
	float srcH = blockWidth / textureHeight;

	float w = srcW - 0.01f;
	float h = srcH - 0.01f;

	float _x = (x * srcW) + 0.005f;
	float _y = (y * srcH) + 0.005f;

	return glm::vec4(_x, _y + h, w, -h);
}


Block::Block(glm::vec3 _position, BlockType _type)
{
	position = _position;
	type = _type;
}

glm::vec4 Block::GetUV(int x, int y)
{
	return GetUVVerticallyFlipped(x, y);
}