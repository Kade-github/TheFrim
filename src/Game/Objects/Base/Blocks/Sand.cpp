#include "Sand.h"
#include "../../../Scenes/Gameplay.h"
#include "../FallingBlock.h"
#include <Game.h>

Sand::Sand(glm::vec3 _position, bool functional) : Block(_position, BlockType::SAND)
{
	position = _position;

	soundType = SoundType::S_SAND;
	toughness = 1.4;

	func = functional;

	if (func)
		currentChunk = WorldManager::instance->GetChunk(position.x, position.z); // cache this
}

bool Sand::Update(int tick)
{
	if (!func)
		return true;

	if (currentChunk == nullptr)
		return true;

	if (currentChunk->GetBlock(position.x, position.y - 1, position.z) == 0 && !falling)
	{
		falling = true;
		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		std::vector<GameObject::VVertex> vertices;
		std::vector<unsigned int> indices;

		for (BlockFace bf : faces)
		{
			vertices.insert(vertices.end(), bf.vertices.begin(), bf.vertices.end());
			for (int i = 0; i < bf.indices.size(); i++)
				indices.push_back(bf.indices[i] + vertices.size() - bf.vertices.size());
		}

		for (GameObject::VVertex& v : vertices)
			v.position -= position;

		FallingBlock* fb = new FallingBlock(position, t, vertices, indices, BlockType::SAND);
		gp->AddObject(fb);

		currentChunk->ModifyBlock(position.x, position.y, position.z, 0);
	}

	return true;
}