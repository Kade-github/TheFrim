#include "../../LightingManager.h"
#include "../../WorldManager.h"

#include "../../Scenes/Gameplay.h"

#include <Game.h>

#include "Blocks/Grass.h"
#include "Blocks/Dirt.h"
#include "Blocks/Stone.h"
#include "Blocks/Cobblestone.h"
#include "Blocks/Wood.h"
#include "Blocks/Leaves.h"
#include "Blocks/Sand.h"
#include "Blocks/Water.h"
#include "Blocks/CraftingTable.h"
#include "Blocks/WoodenPlanks.h"
#include "Blocks/Glass.h"
#include "Blocks/Bedrock.h"
#include "Blocks/CoalOre.h"
#include "Blocks/IronOre.h"
#include "Blocks/GoldOre.h"
#include "Blocks/DiamondOre.h"
#include "Blocks/NullBlock.h"
#include "Blocks/Torch.h"
#include "Blocks/Furnace.h"
#include "Blocks/RuinedCobblestone.h"
#include "Blocks/RuinedDebris.h"
#include "Blocks/ReinforcedIronBlock.h"
#include "Blocks/Rocket.h"

void Chunk::ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal)
{
	for (GameObject::VVertex& v : vertices)
		v.normal = normal;
}

Block* subChunk::getBlock(int x, int z)
{
	return blocks[x][z];
}

int Chunk::GetBlock(float x, float y, float z)
{
	float _x = x;
	float _z = z;

	glm::vec3 w = WorldToChunk(glm::vec3(x, y, z));

	if (_x >= CHUNK_SIZE || _x < 0)
		_x = (int)w.x;

	if (_z >= CHUNK_SIZE || _z < 0)
		_z = (int)w.z;

	if (_x == 16)
		_x = 15;
	if (_z == 16)
		_z = 15;

	if (_x >= CHUNK_SIZE)
		return 0;

	if (_z >= CHUNK_SIZE)
		return 0;

	if (_x < 0)
		return 0;

	if (_z < 0)
		return 0;


	if (y < 0 || y > CHUNK_HEIGHT - 1)
		return 0;

	return myData.bChunk.blocks[(int)_x][(int)_z][(int)y];
}

int Chunk::GetHighestBlock(float x, float z, bool water)
{
	float _x = x;
	float _z = z;

	glm::vec3 w = WorldToChunk(glm::vec3(x, 0, z));

	if (_x >= CHUNK_SIZE || _x < 0)
		_x = (int)w.x;

	if (_z >= CHUNK_SIZE || _z < 0)
		_z = (int)w.z;

	if (_x == 16)
		_x = 15;
	if (_z == 16)
		_z = 15;

	if (_x >= CHUNK_SIZE)
		return 0;

	if (_z >= CHUNK_SIZE)
		return 0;

	if (_x < 0)
		return 0;

	if (_z < 0)
		return 0;

	for (int y = CHUNK_HEIGHT - 1; y > -1; y--)
	{
		int data = myData.bChunk.blocks[(int)_x][(int)_z][y];

		if (!water)
		{
			if (data > 0)
				return y;
		}
		else
		{
			if (data > 0 && data != WATER && data != LEAVES && data != TORCH && data != ROCKET)
				return y;
		}
	}

	return -1;
}


bool Chunk::InterchunkDoesBlockExist(float x, float y, float z)
{
	int _x = x;
	int _z = z;

	if (_x < 0)
	{
		// left chunk

		left = WorldManager::instance->GetChunk(position.x - CHUNK_SIZE, position.z);

		if (left != nullptr)
			return left->GetBlock(0, y, _z) > 0;
		else
			return true;
	}

	if (_x >= CHUNK_SIZE)
	{
		// right chunk

		right = WorldManager::instance->GetChunk(position.x + CHUNK_SIZE, position.z);

		if (right != nullptr)
			return right->GetBlock(CHUNK_SIZE - 1, y, z) > 0;
		else
			return true;

	}

	if (_z < 0)
	{
		// front chunk

		front = WorldManager::instance->GetChunk(position.x, position.z - CHUNK_SIZE);

		if (front != nullptr)
			return front->GetBlock(x, y, CHUNK_SIZE - 1) > 0;
		else
			return true;
	}

	if (_z >= CHUNK_SIZE)
	{
		// back chunk

		back = WorldManager::instance->GetChunk(position.x, position.z + CHUNK_SIZE);

		if (back != nullptr)
			return back->GetBlock(x, y, 0) > 0;
		else
			return true;
	}

	return GetBlock(x, y, z) > 0;
}

int Chunk::GetBlockInterchunk(float x, float y, float z)
{
	int _x = x;
	int _z = z;

	if (_x < 0)
	{
		// left chunk

		left = WorldManager::instance->GetChunk(position.x - CHUNK_SIZE, position.z);

		if (left != nullptr)
			return left->GetBlock(CHUNK_SIZE - 1, y, _z);
		else
			return 0;
	}

	if (_x >= CHUNK_SIZE)
	{
		// right chunk

		right = WorldManager::instance->GetChunk(position.x + CHUNK_SIZE, position.z);

		if (right != nullptr)
			return right->GetBlock(0, y, z);
		else
			return 0;

	}

	if (_z < 0)
	{
		// front chunk

		front = WorldManager::instance->GetChunk(position.x, position.z - CHUNK_SIZE);

		if (front != nullptr)
			return front->GetBlock(x, y, CHUNK_SIZE - 1);
		else
			return 0;
	}

	if (_z >= CHUNK_SIZE)
	{
		// back chunk

		back = WorldManager::instance->GetChunk(position.x, position.z + CHUNK_SIZE);

		if (back != nullptr)
			return back->GetBlock(x, y, 0);
		else
			return 0;
	}

	return GetBlock(x, y, z);
}

int Chunk::GetBlockNoCheck(float x, float y, float z)
{
	int _x = x;
	int _z = z;
	glm::vec3 w = WorldToChunk(glm::vec3(x, y, z));

	if (_x < CHUNK_SIZE && _x >= 0)
		w.x = _x;

	if (_z < CHUNK_SIZE && _z >= 0)
		w.z = _z;

	_x = w.x;
	_z = w.z;

	if (_x > CHUNK_SIZE - 1)
		return 0;

	if (_z > CHUNK_SIZE - 1)
		return 0;

	if (_x < 0)
		return 0;

	if (_z < 0)
		return 0;


	if (y < 0 || y > CHUNK_HEIGHT - 1)
		return 0;

	return myData.bChunk.blocks[_x][_z][(int)y];
}

int Chunk::GetBlockRaw(float x, float y, float z)
{
	int _x = x;
	int _z = z;

	return myData.bChunk.blocks[_x][_z][(int)y];

}

bool Chunk::DoesBlockExist(float x, float y, float z)
{
	if (y < 0 || y > CHUNK_HEIGHT - 1)
		return false;

	return GetBlock(x, y, z) > 0; // if its anything under 0 (thats impossible, uint etc), and if its 0 it's air.
}

void Chunk::CreateOtherSubchunks(glm::vec3 w)
{

	Chunk* c1 = WorldManager::instance->GetChunk(position.x - CHUNK_SIZE, position.z);

	if (c1 != nullptr)
		c1->modified = true;

	Chunk* c2 = WorldManager::instance->GetChunk(position.x + CHUNK_SIZE, position.z);

	if (c2 != nullptr)
		c2->modified = true;

	Chunk* c3 = WorldManager::instance->GetChunk(position.x, position.z - CHUNK_SIZE);

	if (c3 != nullptr)
		c3->modified = true;

	Chunk* c4 = WorldManager::instance->GetChunk(position.x, position.z + CHUNK_SIZE);

	if (c4 != nullptr)
		c4->modified = true;
}

void Chunk::ModifyBlock(float x, float y, float z, int id)
{
	Gameplay* gp = (Gameplay*)Game::instance->currentScene;
	glm::vec3 w = WorldToChunk(glm::vec3(x, y, z));

	if (x >= 0 && x < CHUNK_SIZE - 1)
		w.x = (int)x;

	if (z >= 0 && z < CHUNK_SIZE - 1)
		w.z = (int)z;

	if (w.x > CHUNK_SIZE - 1)
		return;

	if (w.z > CHUNK_SIZE - 1)
		return;

	if (id <= 0) // destroyed block
	{
		if (myData.bChunk.blocks[(int)w.x][(int)w.z][(int)w.y] == LEAVES)
		{
			// 5% chance of dropping an apple

			if (rand() % 100 < 5)
			{
				Data::InventoryItem apple = { Data::ITEM_APPLE, 1 };
				gp->dim->SpawnItem(glm::vec3(x, y, z), apple);
			}
		}

		myData.removeBlockData(w.x, w.y, w.z);
		myData.placeBlock(w.x, w.y, w.z, 0);
	}
	else
		myData.placeBlock(w.x, w.y, w.z, id);

	modified = true;

	CreateOtherSubchunks(w);
}

void Chunk::PlaceBlock(float x, float y, float z, Block* b)
{
	if (b == nullptr)
		return;

	Gameplay* gp = (Gameplay*)Game::instance->currentScene;
	glm::vec3 w = WorldToChunk(glm::vec3(x, y, z));

	if (x >= 0 && x < CHUNK_SIZE - 1)
		w.x = (int)x;

	if (z >= 0 && z < CHUNK_SIZE - 1)
		w.z = (int)z;

	if ((int)w.x > CHUNK_SIZE - 1)
		return;

	if ((int)w.z > CHUNK_SIZE - 1)
		return;

	myData.addBlockData(b->data, w.x, w.y, w.z);
	myData.placeBlock(w.x, w.y, w.z, b->type);


	modified = true;

	CreateOtherSubchunks(w);
}

// this is made confusingly. I'm sorry.
void Chunk::CreateFaces(Block* b)
{
	if (b == nullptr)
		return;

	// check adjacent blocks
	bool top = true;
	bool bottom = true;
	bool left = true;
	bool right = true;
	bool front = true;
	bool back = true;

	if (b->type == FURNACE)
	{
		Furnace* f = (Furnace*)b;

		Data::DataTag t = b->data.GetTag("ticksLeft");

		if (t.IsReal() && std::stoi(t.value) >= 0)
			f->light = true;
	}

	int x = b->position.x - position.x;
	int y = b->position.y;
	int z = b->position.z - position.z;

	int t = GetBlock(x, y + 1, z);
	// in our chunk
	if (t > 0 && ((t != WATER && t != GLASS && t != TORCH && t != ROCKET) || b->transparent))
		top = false;
	t = GetBlock(x, y - 1, z);
	if (t > 0 && ((t != WATER && t != GLASS && t != TORCH && t != ROCKET) || b->transparent))
		bottom = false;
	t = GetBlockInterchunk(x + 1, y, z);
	if (t > 0 && ((t != WATER && t != GLASS && t != TORCH && t != ROCKET) || b->transparent))
		left = false;
	t = GetBlockInterchunk(x - 1, y, z);
	if (t > 0 && ((t != WATER && t != GLASS && t != TORCH && t != ROCKET) || b->transparent))
		right = false;
	t = GetBlockInterchunk(x, y, z - 1);
	if (t > 0 && ((t != WATER && t != GLASS && t != TORCH && t != ROCKET) || b->transparent))
		front = false;
	t = GetBlockInterchunk(x, y, z + 1);
	if (t > 0 && ((t != WATER && t != GLASS && t != TORCH && t != ROCKET) || b->transparent))
		back = false;

	// create faces

	b->faces.clear();

	if (top)
	{
		BlockFace f = b->CreateTopFace();
		f.type = 0;
		ApplyNormal(f.vertices, glm::vec3(0, 1.0f, 0));
		f.position = f.vertices[1].position;
		// center
		f.position += (f.vertices[1].position - f.vertices[2].position) / 2.0f;
		b->faces.push_back(f);
	}

	if (bottom)
	{
		BlockFace f = b->CreateBottomFace();
		f.type = 1;

		ApplyNormal(f.vertices, glm::vec3(0, -1.0f, 0));
		f.position = f.vertices[1].position;
		// center
		f.position += (f.vertices[1].position - f.vertices[2].position) / 2.0f;
		b->faces.push_back(f);
	}

	if (left)
	{
		BlockFace f = b->CreateLeftFace();
		f.type = 2;
		ApplyNormal(f.vertices, glm::vec3(1.0f, 0, 0));
		f.position = f.vertices[1].position;
		// center
		f.position += (f.vertices[1].position - f.vertices[2].position) / 2.0f;
		b->faces.push_back(f);
	}

	if (right)
	{
		BlockFace f = b->CreateRightFace();
		f.type = 3;
		ApplyNormal(f.vertices, glm::vec3(-1.0f, 0, 0));
		f.position = f.vertices[1].position;
		// center
		f.position += (f.vertices[1].position - f.vertices[2].position) / 2.0f;
		b->faces.push_back(f);
	}

	if (front)
	{
		BlockFace f = b->CreateFrontFace();
		f.type = 4;
		ApplyNormal(f.vertices, glm::vec3(0, 0, -1.0f));
		f.position = f.vertices[1].position;
		// center
		f.position += (f.vertices[1].position - f.vertices[2].position) / 2.0f;
		b->faces.push_back(f);
	}

	if (back)
	{
		BlockFace f = b->CreateBackFace();
		f.type = 5;
		ApplyNormal(f.vertices, glm::vec3(0, 0, 1.0f));
		f.position = f.vertices[1].position;
		// center
		f.position += (f.vertices[1].position - f.vertices[2].position) / 2.0f;
		b->faces.push_back(f);
	}
}

void Chunk::RenderSubChunk(subChunk& sbc)
{
	if (sbc.y == -1)
		return;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			Block* block = sbc.getBlock(x, z);
			if (block == nullptr)
				continue;

			if (block->isModel)
			{
				models.push_back(block);
				continue;
			}

			if (block->transparent)
			{
				CreateFaces(block);

				for (BlockFace f : block->faces)
				{
					transparentVertices.insert(transparentVertices.end(), f.vertices.begin(), f.vertices.end());
					for (int i = 0; i < f.indices.size(); i++)
						transparentIndices.push_back(f.indices[i] + transparentVertices.size() - f.vertices.size());
				}
				continue;
			}

			CreateFaces(block);

			for (BlockFace f : block->faces)
			{
				vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());
				for (int i = 0; i < f.indices.size(); i++)
					indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
			}
		}
	}
}

void Chunk::RenderSubChunks()
{
	vertices.clear();
	indices.clear();

	transparentVertices.clear();
	transparentIndices.clear();

	models.clear();

	std::lock_guard<std::mutex> lock(Data::World::worldMutex);
	for (int i = 0; i < subChunks.size(); i++)
	{
		subChunk& sbc = subChunks[i];
		if (sbc.y == -1)
			continue;

		RenderSubChunk(sbc);
	}

}

glm::vec3 Chunk::WorldToChunk(glm::vec3 pos)
{
	return glm::vec3(std::abs(pos.x - position.x), pos.y, std::abs(pos.z - position.z));
}


Chunk::Chunk(Texture* _txp, glm::vec3 _pos) : GameObject(_pos)
{
	position = _pos;
	txp = _txp;
}

subChunk& Chunk::GetSubChunk(int y)
{
	static subChunk s;
	for (int i = 0; i < subChunks.size(); i++)
	{
		subChunk& sbc = subChunks[i];

		if (sbc.y == -1)
			continue;

		if (sbc.y == y)
			return sbc;
	}
	s.y = -1;
	return s;
}

Data::Chunk Chunk::GetChunkData()
{
	Data::Chunk c;
	c.x = position.x;
	c.z = position.z;

	if (!myData.isGenerated)
		return c;

	for (int y = 0; y < CHUNK_HEIGHT; y++)
	{
		subChunk& sbc = GetSubChunk(y);

		if (sbc.y == -1) // not loaded
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
					c.bChunk.blocks[x][z][y] = myData.bChunk.blocks[x][z][y];
			}
			continue;
		}

		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				if (sbc.blocks[x][z] != nullptr)
					c.bChunk.blocks[x][z][y] = sbc.blocks[x][z]->type;
				else
					c.bChunk.blocks[x][z][y] = 0;
			}
		}
	}

	c.data = myData.data;

	return c;
}

bool Chunk::IsInChunk(float x, float z)
{
	if (x < position.x || x >= position.x + CHUNK_SIZE)
		return false;

	if (z < position.z || z >= position.z + CHUNK_SIZE)
		return false;

	return true;
}


void Chunk::RenderSubChunkShadow(subChunk& sbc)
{
	if (sbc.y == -1)
		return;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			Block* block = sbc.getBlock(x, z);
			if (block == nullptr)
				continue;

			if (block->transparent || block->faces.size() == 0)
				continue;

			if (block->isModel)
				continue;

			for (BlockFace f : block->faces)
			{
				// check light

				int light = LightingManager::GetInstance()->GetLightLevel(block->position + f.vertices[0].normal);

				if (light < 10)
				{
					// shadow overlay
					glm::vec4 shadowUV = glm::vec4();
					if (light > 7)
						shadowUV = block->t->spriteSheet.GetUVFlip("Shadow25");
					else if (light > 4)
						shadowUV = block->t->spriteSheet.GetUVFlip("Shadow50");
					else if (light > 1)
						shadowUV = block->t->spriteSheet.GetUVFlip("Shadow75");
					else
						shadowUV = block->t->spriteSheet.GetUVFlip("shadow");

					std::vector<GameObject::VVertex> sV = f.vertices;


					// set uv

					sV[0].uv = shadowUV;
					sV[1].uv = glm::vec2(shadowUV.x + shadowUV.z, shadowUV.y);
					sV[2].uv = glm::vec2(shadowUV.x, shadowUV.y + shadowUV.w);
					sV[3].uv = glm::vec2(shadowUV.x + shadowUV.z, shadowUV.y + shadowUV.w);

					shadowVertices.insert(shadowVertices.end(), sV.begin(), sV.end());
					for (int i = 0; i < f.indices.size(); i++)
						shadowIndices.push_back(f.indices[i] + shadowVertices.size() - sV.size());
				}
			}
		}
	}
}

void Chunk::RenderSubChunksShadow()
{
	std::lock_guard<std::mutex> lock(Data::World::worldMutex);
	shadowVertices.clear();
	shadowIndices.clear();
	for (int i = 0; i < subChunks.size(); i++)
	{
		subChunk& sbc = subChunks[i];
		if (sbc.y == -1)
			continue;

		RenderSubChunkShadow(sbc);
	}
}

subChunk Chunk::CreateSubChunk(int y)
{
	subChunk sbc;

	sbc.y = y;

	bool isOccluded = true;

	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			int type = GetBlock(x, y + 1, z);
			int type2 = GetBlock(x, y - 1, z);
			int type3 = GetBlockInterchunk(x + 1, y, z);
			int type4 = GetBlockInterchunk(x - 1, y, z);
			int type5 = GetBlockInterchunk(x, y, z + 1);
			int type6 = GetBlockInterchunk(x, y, z - 1);

			if (type <= 0 || (type == WATER || type == GLASS))
				isOccluded = false;
			else if (type2 <= 0 || (type2 == WATER || type2 == GLASS))
				isOccluded = false;
			else if (type3 <= 0 || (type3 == WATER || type3 == GLASS))
				isOccluded = false;
			else if (type4 <= 0 || (type4 == WATER || type4 == GLASS))
				isOccluded = false;
			else if (type5 <= 0 || (type5 == WATER || type5 == GLASS))
				isOccluded = false;
			else if (type6 <= 0 || (type6 == WATER || type6 == GLASS))
				isOccluded = false;

			if (!isOccluded)
				break;
		}
	}

	bool hasBlocks = false;

	if (!isOccluded)
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int z = 0; z < CHUNK_SIZE; z++)
			{
				int id = myData.bChunk.blocks[x][z][y];

				if (id <= 0)
					continue;

				Data::BlockData data = myData.getBlockData(x, y, z);

				Block* b = CreateBlock(x, y, z, id, data);

				sbc.blocks[x][z] = b;
				hasBlocks = true;
			}
		}
	}
	else
	{
		DestroySubChunk(sbc);
		return {};
	}

	if (!hasBlocks)
	{
		DestroySubChunk(sbc);
		return {};
	}

	return sbc;
}

Block* Chunk::CreateBlock(int x, int y, int z, int id, Data::BlockData data)
{
	Block* block;
	Data::DataTag dataOne;
	Data::DataTag dataTwo;
	bool source = false;

	switch (id)
	{
	case GRASS:
		block = new Grass(position + glm::vec3(x, y, z));
		break;
	case STONE:
		block = new Stone(position + glm::vec3(x, y, z));
		break;
	case COBBLESTONE:
		block = new Cobblestone(position + glm::vec3(x, y, z));
		break;
	case WOOD:
		block = new Wood(position + glm::vec3(x, y, z));
		break;
	case LEAVES:
		block = new Leaves(position + glm::vec3(x, y, z));
		break;
	case SAND:
		block = new Sand(position + glm::vec3(x, y, z));
		break;
	case WATER:
		dataOne = data.GetTag("source");
		if (!dataOne.IsReal())
			dataOne.SetValue("true");
		dataTwo = data.GetTag("strength");
		if (!dataTwo.IsReal())
			dataTwo.SetValue("8");

		source = std::string(dataOne.value) == "true";

		block = new Water(position + glm::vec3(x, y, z), std::stoi(dataTwo.value), source);
		block->data = data;
		break;
	case CRAFTINGTABLE:
		block = new CraftingTable(position + glm::vec3(x, y, z));
		break;
	case WOODENPLANKS:
		block = new WoodenPlank(position + glm::vec3(x, y, z));
		break;
	case GLASS:
		block = new Glass(position + glm::vec3(x, y, z));
		break;
	case BEDROCK:
		block = new Bedrock(position + glm::vec3(x, y, z));
		break;
	case COAL_ORE:
		block = new CoalOre(position + glm::vec3(x, y, z));
		break;
	case IRON_ORE:
		block = new IronOre(position + glm::vec3(x, y, z));
		break;
	case GOLD_ORE:
		block = new GoldOre(position + glm::vec3(x, y, z));
		break;
	case DIAMOND_ORE:
		block = new DiamondOre(position + glm::vec3(x, y, z));
		break;
	case DIRT:
		block = new Dirt(position + glm::vec3(x, y, z));
		break;
	case TORCH:
		block = new Torch(position + glm::vec3(x, y, z));
		block->data = data;
		break;
	case FURNACE:
		block = new Furnace(position + glm::vec3(x, y, z));
		block->data = data;
		break;
	case RUINED_COBBLESTONE:
		block = new RuinedCobblestone(position + glm::vec3(x, y, z));
		break;
	case RUINED_DEBRIS:
		block = new RuinedDebris(position + glm::vec3(x, y, z));
		break;
	case REINFORCED_IRON_BLOCK:
		block = new ReinforcedIronBlock(position + glm::vec3(x, y, z));
		break;
	case ROCKET:
		block = new Rocket(position + glm::vec3(x, y, z));
		break;
	default:
		block = new NullBlock(position + glm::vec3(x, y, z));
		break;
	}
	block->chunkPosition = glm::vec3(x, y, z);
	block->t = txp;

	return block;
}

void Chunk::DestroySubChunk(int y)
{
	subChunk& sbc = GetSubChunk(y);

	if (sbc.y == -1)
		return;

	DestroySubChunk(sbc);
}

void Chunk::DestroySubChunk(subChunk& c)
{
	for (int x = 0; x < CHUNK_SIZE; x++)
	{
		for (int z = 0; z < CHUNK_SIZE; z++)
		{
			Block* b = c.blocks[x][z];

			if (b == nullptr) {
				c.blocks[x][z] = nullptr;
				continue;
			}

			b->faces.clear();
			if (b->data.tags.size() > 0)
			{
				myData.setBlockData(b->chunkPosition.x, b->chunkPosition.y, b->chunkPosition.z, b->data);
			}
			b->Destroy();
			delete b;
			c.blocks[x][z] = nullptr;
		}
	}
}

void Chunk::DestroySubChunks()
{
	std::lock_guard<std::mutex> lock(Data::World::worldMutex);
	for (int i = CHUNK_HEIGHT; i > -1; i--)
		DestroySubChunk(i);

	models.clear();

	subChunks.clear();
}

void Chunk::CreateSubChunks()
{
	DestroySubChunks();
	std::lock_guard<std::mutex> lock(Data::World::worldMutex);

	for (int y = CHUNK_HEIGHT - 1; y > -1; y--)
	{
		subChunk sbc = CreateSubChunk(y);
		if (sbc.y != -1)
			subChunks.push_back(sbc);
	}

}

void Chunk::SetBuffer()
{
	if (vertices.size() != 0)
	{
		// bind first vertex array
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GameObject::VVertex), vertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// position attribute
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
		glEnableVertexAttribArray(0);

		// uv attribute
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));
		glEnableVertexAttribArray(1);

		// normal attribute

		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, normal));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);

		size = indices.size();
	}

	if (modelsPresent.size() != 0)
		modelsPresent.clear();

	for (auto block : models)
	{
		block->LoadModel();
		block->m.position = block->position;
		block->Mo();
		modelsPresent.push_back(block->m);
	}

	vertices.clear();
	indices.clear();
}

void Chunk::SetTransparentBuffer()
{
	if (transparentVertices.size() == 0)
		return;

	// bind transparent vertex array
	glBindVertexArray(TRANSPARENTVAO);

	glBindBuffer(GL_ARRAY_BUFFER, TRANSPARENTVBO);
	glBufferData(GL_ARRAY_BUFFER, transparentVertices.size() * sizeof(GameObject::VVertex), transparentVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TRANSPARENTEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, transparentIndices.size() * sizeof(unsigned int), transparentIndices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));
	glEnableVertexAttribArray(1);

	// normal attribute

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, normal));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	transparentSize = transparentIndices.size();

	transparentIndices.clear();
	transparentVertices.clear();
}

void Chunk::SetShadowBuffer()
{
	if (shadowVertices.size() == 0)
		return;

	// bind shadow vertex array
	glBindVertexArray(SHADOWVAO);

	glBindBuffer(GL_ARRAY_BUFFER, SHADOWVBO);
	glBufferData(GL_ARRAY_BUFFER, shadowVertices.size() * sizeof(GameObject::VVertex), shadowVertices.data(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SHADOWEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, shadowIndices.size() * sizeof(unsigned int), shadowIndices.data(), GL_STATIC_DRAW);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)0);
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, uv));
	glEnableVertexAttribArray(1);

	// normal attribute

	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(GameObject::VVertex), (void*)offsetof(GameObject::VVertex, normal));
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);

	shadowSize = shadowIndices.size();
	shadowVertices.clear();
	shadowIndices.clear();
}

void Chunk::Init()
{
	if (inited)
		return;
	inited = true;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glGenVertexArrays(1, &TRANSPARENTVAO);
	glGenBuffers(1, &TRANSPARENTVBO);
	glGenBuffers(1, &TRANSPARENTEBO);

	glGenVertexArrays(1, &SHADOWVAO);
	glGenBuffers(1, &SHADOWVBO);
	glGenBuffers(1, &SHADOWEBO);

}

void Chunk::Destroy()
{
	DestroySubChunks();

	if (!inited)
		return;

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDeleteVertexArrays(1, &SHADOWVAO);
	glDeleteBuffers(1, &SHADOWVBO);
	glDeleteBuffers(1, &SHADOWEBO);

	glDeleteVertexArrays(1, &TRANSPARENTVAO);
	glDeleteBuffers(1, &TRANSPARENTVBO);
	glDeleteBuffers(1, &TRANSPARENTEBO);

}

void Chunk::Unload()
{
	DestroySubChunks();

	vertices.clear();
	indices.clear();

	transparentVertices.clear();
	transparentIndices.clear();

	shadowVertices.clear();
	shadowIndices.clear();

	models.clear();
	modelsPresent.clear();

	size = 0;
	shadowSize = 0;
}

void Chunk::DrawRegular()
{
	if (!isRendered || size == 0)
		return;

	Shader* s = Game::instance->shader;

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(VAO); // regular faces
	txp->Bind();
	s->Bind();
	s->SetUniform1f("lightLevel", 10.0f);

	glm::mat4 model = glm::mat4(1.0f);

	s->SetUniformMat4f("model", &model[0][0]);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, 0);

	txp->Unbind();
	s->Unbind();

	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
}

void Chunk::DrawModels()
{
	if (!isRendered || modelsPresent.size() == 0)
		return;

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);

	Shader* s = Game::instance->shader;

	for (int i = 0; i < modelsPresent.size(); i++)
	{
		auto&& m = modelsPresent[i];

		int lightLevel = LightingManager::GetInstance()->GetLightLevel(m.position);

		s->Bind();

		s->SetUniform1f("lightLevel", lightLevel);

		s->Unbind();

		m.Draw();

		s->Bind();

		s->SetUniform1f("lightLevel", 10.0f);

		s->Unbind();
	}

	glDisable(GL_CULL_FACE);
}

void Chunk::DrawTransparent()
{
	if (!isRendered || transparentSize == 0)
		return;

	Shader* s = Game::instance->shader;

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(TRANSPARENTVAO); // transparent faces
	txp->Bind();
	s->Bind();
	s->SetUniform1f("lightLevel", 10.0f);

	glm::mat4 model = glm::mat4(1.0f);

	s->SetUniformMat4f("model", &model[0][0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, TRANSPARENTEBO);

	glDrawElements(GL_TRIANGLES, transparentSize, GL_UNSIGNED_INT, 0);

	txp->Unbind();
	s->Unbind();

	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
}

void Chunk::DrawShadows()
{
	if (!isRendered || shadowSize == 0)
		return;

	Shader* s = Game::instance->shader;

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(SHADOWVAO); // shadow faces
	txp->Bind();
	s->Bind();
	s->SetUniform1f("lightLevel", 10.0f);

	glm::mat4 model = glm::mat4(1.0f);

	s->SetUniformMat4f("model", &model[0][0]);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, SHADOWEBO);

	glDrawElements(GL_TRIANGLES, shadowSize, GL_UNSIGNED_INT, 0);

	s->Unbind();
	txp->Unbind();

	glBindVertexArray(0);

	glDisable(GL_CULL_FACE);
}

void Chunk::UpdateChunk(int tick)
{
	if (modified)
	{
		auto gp = (Gameplay*)Game::instance->currentScene;

		gp->QueueLoadBlocks(this);

		gp->QueueShadow(this);
		modified = false;
	}

	if (subChunks.size() == 0)
		return;

	{
		std::lock_guard<std::mutex> lock(Data::World::worldMutex);
		for (int i = 0; i < subChunks.size(); i++)
		{
			subChunk& sbc = subChunks[i];

			if (sbc.y == -1)
				continue;

			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				for (int z = 0; z < CHUNK_SIZE; z++)
				{
					Block* b = sbc.blocks[x][z];

					if (b == nullptr)
					{
						continue;
					}

					if (!b->updateable)
					{
						continue;
					}

					b->Update(tick);
				}
			}

		}
	}
}