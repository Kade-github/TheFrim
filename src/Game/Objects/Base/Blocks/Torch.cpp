#include "Torch.h"
#include "../../../LightingManager.h"
#include "../../../Scenes/Gameplay.h"
#include <Game.h>

Torch::Torch(glm::vec3 _position) : Block(_position, BlockType::TORCH)
{
	position = _position;

	soundType = SoundType::S_STONE;
	toughness = 9.0f;
	isModel = true;
	transparent = true;
	collidable = false;

	LightingManager::GetInstance()->AddLight(position, 10);

	currentChunk = WorldManager::instance->GetChunk(position.x, position.z); // cache this
}


void Torch::Destroy()
{
	LightingManager::GetInstance()->RemoveLight(position);
}

void Torch::LoadModel()
{
	m = Model("Assets/Models/torchfrim.obj");
	m.LoadUVMap("torch_frim");
	m.scale = glm::vec3(0.35f, 0.35f, 0.35f);
}

void Torch::Mo()
{
	Data::DataTag facing = data.GetTag("facing");

	m.position += glm::vec3(0.5f, 0.4f, 0.5f);

	if (facing.IsReal())
	{
		int face = std::stoi(facing.value);

		switch (face)
		{
		case 1:
			m.rotateAxis = glm::vec3(0, 0, 1);
			m.angle = -25;
			m.position += glm::vec3(-0.35f, 0.25f, 0);
			break;
		case 2:
			m.rotateAxis = glm::vec3(0, 0, 1);
			m.angle = 25;
			m.position += glm::vec3(0.35f, 0.25f, 0);
			break;
		case 3:
			m.rotateAxis = glm::vec3(1, 0, 0);
			m.angle = -25;
			m.position += glm::vec3(0, 0.25f, 0.35f);
			break;
		case 4:
			m.rotateAxis = glm::vec3(1, 0, 0);
			m.angle = 25;
			m.position += glm::vec3(0, 0.25f, -0.35f);
			break;
		default:
			m.rotateAxis = glm::vec3(0, 1, 0);
			m.angle = 0;
			break;
		}
		_facing = face;
	}
}

bool Torch::Update(int tick)
{
	if (broken)
		return true;

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;
	Data::InventoryItem i(Data::ITEM_TORCH, 1);
	switch (_facing)
	{
	case 0:
		if (currentChunk->GetBlock(position.x, position.y - 1, position.z) == 0)
		{
			currentChunk->ModifyBlock(chunkPosition.x, chunkPosition.y, chunkPosition.z, 0);
			gp->dim->SpawnItem(position + glm::vec3(0.5f,0.5f,0.5f), i);
			broken = true;
		}
		break;
	case 1:
		if (currentChunk->GetBlock(position.x - 1, position.y, position.z) == 0)
		{
			currentChunk->ModifyBlock(chunkPosition.x, chunkPosition.y, chunkPosition.z, 0);
			gp->dim->SpawnItem(position + glm::vec3(0.5f, 0.5f, 0.5f), i);
			broken = true;
		}
		break;
	case 2:
		if (currentChunk->GetBlock(position.x + 1, position.y, position.z) == 0)
		{
			currentChunk->ModifyBlock(chunkPosition.x, chunkPosition.y, chunkPosition.z, 0);
			gp->dim->SpawnItem(position + glm::vec3(0.5f, 0.5f, 0.5f), i);
			broken = true;
		}
		break;
	case 3:
		if (currentChunk->GetBlock(position.x, position.y, position.z + 1) == 0)
		{
			currentChunk->ModifyBlock(chunkPosition.x, chunkPosition.y, chunkPosition.z, 0);
			gp->dim->SpawnItem(position + glm::vec3(0.5f, 0.5f, 0.5f), i);
			broken = true;
		}
		break;
	case 4:
		if (currentChunk->GetBlock(position.x, position.y, position.z - 1) == 0)
		{
			currentChunk->ModifyBlock(chunkPosition.x, chunkPosition.y, chunkPosition.z, 0);
			gp->dim->SpawnItem(position + glm::vec3(0.5f, 0.5f, 0.5f), i);
			broken = true;
		}
		break;
	}

	return true;
}
