#include "../../Scenes/Gameplay.h"
#include <Game.h>
#include "DroppedItem.h"
#include <glm/gtx/rotate_vector.hpp>
#include "../../LightingManager.h"
#include <Helpers/StringTools.h>
#include "../../MusicManager.h"

bool firstMouse = false;

float lastX = 400, lastY = 300;

void Player::ApplyNormal(std::vector<GameObject::VVertex>& vertices, glm::vec3 normal)
{
	for (GameObject::VVertex& v : vertices)
		v.normal = normal;
}

void Player::Hurt(float damage)
{
	MusicManager::GetInstance()->PlaySFX("hurt", "hurtSFX");
	playerData.health -= damage;

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->UpdateHearts();

	CameraShake(0.25f);
}

void Player::Heal(float amount)
{
	playerData.health += amount;

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->UpdateHearts();
}

void Player::CameraShake(float amount)
{
	_shake = amount;
}

void Player::SetItem(Data::InventoryItem item, int x, int y)
{
	playerData.SetInventoryItem(x, y, item);

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->UpdateHotbar();
}

void Player::ToggleInventory()
{
	_inInventory = !_inInventory;

	if (!_inInventory)
		firstMouse = true;

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->InventoryShown(_inInventory);
}

void Player::ToggleCraftingTable()
{
	_inInventory = !_inInventory;

	if (!_inInventory)
		firstMouse = true;

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->ShowCraftingTable(_inInventory);
}

void Player::TogglePauseMenu()
{
	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->ShowPauseMenu(!Hud::GamePaused);
	if (Hud::GamePaused)
		firstMouse = true;
}

void Player::OnScroll(double x, double y)
{
	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	if (y > 0)
	{
		int n = scene->hud->selected - 1;
		if (n < 0)
			scene->hud->SetSelected(8);
		else
			scene->hud->SetSelected(n);

		scene->hud->UpdateHotbar();
	}
	else
	{
		int n = scene->hud->selected + 1;
		if (n > 8)
			scene->hud->SetSelected(0);
		else
			scene->hud->SetSelected(n);

		scene->hud->UpdateHotbar();
	}

}

void Player::DrawBlockBreak(BlockFace f)
{
	vertices.insert(vertices.end(), f.vertices.begin(), f.vertices.end());

	for (int i = 0; i < f.indices.size(); i++)
		indices.push_back(f.indices[i] + vertices.size() - f.vertices.size());
}

void Player::SetBreakBuffer()
{
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

	glBindVertexArray(0);

}

void Player::RenderBreak()
{
	SetBreakBuffer();

	WorldManager* wm = WorldManager::instance;

	Shader* shader = Game::instance->shader;

	shader->Bind();
	wm->texturePack->Bind();

	glm::mat4 model = glm::mat4(1.0f);

	shader->SetUniformMat4f("model", &model[0][0]);

	glEnable(GL_DEPTH_CLAMP);
	glEnable(GL_CULL_FACE);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glDisable(GL_DEPTH_CLAMP);
	glDisable(GL_CULL_FACE);

	wm->texturePack->Unbind();
	shader->Unbind();
}

Player::Player(glm::vec3 pos) : Entity(pos)
{
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
}

void Player::Draw()
{
	if (goToTop)
	{
		Chunk* c = WorldManager::instance->GetChunk(position.x, position.z);

		if (c != nullptr)
		{
			int y = c->GetHighestBlock(position.x, position.z);

			if (y != -1)
			{
				position.y = y + 1;
				goToTop = false;
			}
		}
	}

	if (instantBreak)
		playerData.health = 90000;

	Camera* camera = Game::instance->GetCamera();
	float p = camera->pitch;

	if (!_inInventory && !Hud::GamePaused)
	{
		glm::vec2 mousePos = Game::instance->GetCursorPos(false);

		float x = mousePos.x;
		float y = mousePos.y;

		if (firstMouse)
		{
			p = playerData.pitch;
			yaw = playerData.yaw;
			lastX = x;
			lastY = y;
			firstMouse = false;
		}

		float xoffset = x - lastX;
		float yoffset = lastY - y; // reversed since y-coordinates go from bottom to top

		lastX = x;
		lastY = y;

		const float sensitivity = 0.1f;
		xoffset *= sensitivity;
		yoffset *= sensitivity;
		p += yoffset;
		if (p >= 89.0f)
			p = 89.0f;
		else if (p <= -89.0f)
			p = -89.0f;

		yaw += xoffset;

		if (wasDead && playerData.health <= 0) // respawn
		{
			playerData.health = PLAYER_MAX_HEALTH;
			playerData.air = PLAYER_MAX_AIR;

			Gameplay* scene = (Gameplay*)Game::instance->currentScene;

			scene->hud->UpdateHearts();
			scene->hud->UpdateAir();

			goToTop = true;
			wasDead = false;
			position = glm::vec3(0, 100, 0);
			return;
		}

		if (playerData.health <= 0 && !dead) // die
		{
			dead = true;
			// drop all items

			for (int i = 0; i < PLAYER_INVENTORY_WIDTH; i++)
			{
				for (int j = 0; j < PLAYER_INVENTORY_HEIGHT; j++)
				{
					Data::InventoryItem item = playerData.inventory[i][j];

					if (item.type != Data::ITEM_NULL)
					{
						Gameplay* scene = (Gameplay*)Game::instance->currentScene;

						float rX = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
						float rY = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
						float rZ = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5

						scene->dim->SpawnItem(position + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(rX, rY, rZ), item, 10.0f, 1.0f);
					}
					playerData.inventory[i][j] = {};
				}
			}

			// drop all armor

			for (int i = 0; i < 3; i++)
			{
				Data::InventoryItem item = playerData.armor[i];

				if (item.type != Data::ITEM_NULL)
				{
					Gameplay* scene = (Gameplay*)Game::instance->currentScene;

					float rX = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
					float rY = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
					float rZ = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5

					scene->dim->SpawnItem(position + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(rX, rY, rZ), item, 10.0f, 1.0f);
				}
				playerData.armor[i] = {};
			}

			Gameplay* scene = (Gameplay*)Game::instance->currentScene;

			scene->hud->ShowDeathScreen();
			scene->hud->UpdateHotbar();
		}
	}

	bool moving = false;

	float sp = speed;

	if (inWater)
		sp = speed / 2;

	if (!freeCam)
	{
		if (!_inInventory && !Hud::GamePaused)
		{
			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
			{
				forwardVelocity += sp;
				headBop += 10.0f * Game::instance->deltaTime;
				moving = true;
			}

			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
			{
				forwardVelocity -= sp;
				headBop += 10.0f * Game::instance->deltaTime;
				moving = true;
			}

			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
			{
				strafeVelocity -= sp;
				headBop += 10.0f * Game::instance->deltaTime;
				moving = true;
			}

			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
			{
				strafeVelocity += sp;
				headBop += 10.0f * Game::instance->deltaTime;
				moving = true;
			} 

			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS && (isOnGround || inWater))
			{
				if (glfwGetTime() - jumpCooldown < 0.15 && !inWater)
					return;

				jumpCooldown = glfwGetTime();

				downVelocity = jumpStrength;
				if (topWater)
					downVelocity = jumpStrength / 2;
				isOnGround = false;
			}

			if (forwardVelocity > sp)
				forwardVelocity = sp;

			if (forwardVelocity < -sp)
				forwardVelocity = -sp;

			if (strafeVelocity > sp)
				strafeVelocity = sp;

			if (strafeVelocity < -sp)
				strafeVelocity = -sp;
		}
	}
	else if (!_inInventory && !Hud::GamePaused)
	{
		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
			position += camera->cameraFront * 0.25f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
			position -= camera->cameraFront * 0.25f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
			position -= glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * 0.25f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
			position += glm::normalize(glm::cross(camera->cameraFront, camera->cameraUp)) * 0.25f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_SPACE) == GLFW_PRESS)
			position.y += 0.25f;

		if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			position.y -= 0.25f;

		shadow->Draw();

		camera->position = position;
	}

	if (isOnGround && jumpedFrom > -1)
	{
		float distance = jumpedFrom - position.y;

		float fallDamage = (distance - 2) / 2;

		// round to nearest half or whole

		fallDamage = floor((fallDamage * 2) + 0.5f) / 2;

		if (distance < 4)
			fallDamage = 0;



		if (fallDamage > 0 && !inWater)
		{
			MusicManager::GetInstance()->PlaySFX("fall", "fallSFX");
			Hurt(fallDamage);
		}

		jumpedFrom = -1;
	}
	else if (!isOnGround && jumpedFrom <= -1)
	{
		jumpedFrom = position.y;
	}

	if (!Hud::GamePaused)
	{
		if (topWater)
			playerData.air -= Game::instance->deltaTime;
		else if (playerData.air < 10.0f)
			playerData.air += Game::instance->deltaTime;

		if (playerData.air <= 0)
		{
			playerData.air = 2;
			Hurt(0.5f);
		}

		Gameplay* scene = (Gameplay*)Game::instance->currentScene;

		if (playerData.air < 10.0f)
		{
			if ((int)playerData.air != lastAirUpdate)
			{
				lastAirUpdate = (int)playerData.air;
				scene->hud->UpdateAir();
			}
		}
		else
		{
			if (scene->hud->air.size() != 0)
				scene->hud->ClearAir();
		}
	}

	if (!freeCam)
	{
		glm::vec3 storedPos = position;

		camera->position = storedPos;
		camera->position.y -= 0.1;
	}

	if (headBop > 0)
	{
		static bool footstepped = false;
		if (!moving || !isOnGround)
			headBop -= 20.0f * Game::instance->deltaTime;

		if (headBop > 0.8f)
			headBop = 0.8f;

		float s = (sin(glfwGetTime() * 12) * 0.04f) * headBop;

		camera->position.y += s;

		if (s < -0.03 && !footstepped)
		{
			Footstep();
			footstepped = true;
		}

		if (s > 0.03)
			footstepped = false;
	}


	camera->pitch = p;
	camera->yaw = yaw;

	playerData.pitch = p;
	playerData.yaw = yaw;

	if (_shake > 0)
	{
		_shake -= 1.75f * Game::instance->deltaTime;
		camera->position.x += ((rand() % 100) / 100.0f) * _shake;
		camera->position.y += ((rand() % 100) / 100.0f) * _shake;
		camera->position.z += ((rand() % 100) / 100.0f) * _shake;
	}

	camera->SetDirection();

	glm::vec3 ray = position + (camera->cameraFront * 5.0f);

	bool hit = RayTo(ray, true);
	if (hit)
	{
		Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

		if (c != nullptr)
		{
			subChunk& sb = c->GetSubChunk(ray.y);

			if (sb.y != -1)
			{

				glm::vec3 _world = c->WorldToChunk(ray);

				selectedBlock = sb.getBlock(_world.x, _world.z);

				if (selectedBlock != nullptr)
				{
					// get the closest face

					bool stop = false;

					for (BlockFace f : selectedBlock->faces)
					{
						if (stop)
							break;

						float dist = 0;

						if (f.vertices.size() == 0)
							continue;

						switch (f.type)
						{
						case 0:
						case 1:
							dist = abs(f.vertices[0].position.y - ray.y);
							if (dist < 0.1f)
							{
								selectedFace = f;
								stop = true;
							}
							break;
						case 2:
						case 3:
							dist = abs(f.vertices[0].position.x - ray.x);
							if (dist < 0.1f)
							{
								selectedFace = f;
								stop = true;
							}
							break;
						case 4:
						case 5:
							dist = abs(f.vertices[0].position.z - ray.z);
							if (dist < 0.1f)
							{
								selectedFace = f;
								stop = true;
							}
							break;
						}
					}
				}
				else
					selectedBlock = nullptr;

			}
			else
				selectedBlock = nullptr;
		}
		else
			selectedBlock = nullptr;
	}
	else
		selectedBlock = nullptr;


	vertices.clear();
	indices.clear();

	if (!_inInventory && !Hud::GamePaused)
	{
		if (selectedBlock != nullptr && glfwGetMouseButton(Game::instance->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			Gameplay* scene = (Gameplay*)Game::instance->currentScene;

			Data::InventoryItem item = playerData.inventory[scene->hud->selected][PLAYER_INVENTORY_HEIGHT - 1];

			float toughness = selectedBlock->toughness;

			bool correctTool = false;

			switch (selectedBlock->soundType)
			{
				case S_GRASS:
				case S_SAND:
					if (StringTools::Contains(item.tag, "shovel"))
					{
						toughness *= item.breakingPower;
						correctTool = true;
					}
					break;
				case S_WOOD:
					if (StringTools::Contains(item.tag, "axe"))
					{
						toughness *= item.breakingPower;
						correctTool = true;
					}
					break;
				case S_STONE:
					if (StringTools::Contains(item.tag, "pick"))
					{
						toughness *= item.breakingPower;
						correctTool = true;
					}
					break;
				default:
					toughness *= item.breakingPower;
					correctTool = true;
					break;
			}

			if (topWater)
				toughness *= 0.5f;

			if (instantBreak)
			{
				toughness = 1000.0f;
				correctTool = true;
			}

			selectedBlock->breakProgress += (1.0f * Game::instance->deltaTime) * toughness;

			if (selectedBlock->breakProgress >= 1)
			{
				selectedBlock->breakProgress = 0;

				glm::vec3 _world = selectedBlock->position;

				Chunk* c = WorldManager::instance->GetChunk(_world.x, _world.z);

				if (c != nullptr)
				{
					bool giveItem = true;

					switch (selectedBlock->type)
					{
					case LEAVES:
						giveItem = false;
						break;
					case TORCH:
						giveItem = true;
						break;
					}

					switch(selectedBlock->soundType)
					{ 
					case S_STONE:
						if (correctTool)
						{
							switch (selectedBlock->type)
							{
							case STONE:
							case COBBLESTONE:
								if (item.breakingPower >= 2)
									giveItem = true;
								break;
							}
						}
						break;
					}

					if (instantBreak)
						giveItem = true;
					
					if (giveItem)
					{
						Data::InventoryItem item = { selectedBlock->type, 1 };

						switch (selectedBlock->type)
						{
						case GRASS:
							item = { DIRT, 1 };
							break;
						case STONE:
							item = { COBBLESTONE, 1 };
							break;
						case COAL_ORE:
							item = { Data::ITEM_COAL, 1 };
							break;
						case DIAMOND_ORE:
							item = { Data::ITEM_DIAMOND, 1 };
							break;
						}

						Gameplay* scene = (Gameplay*)Game::instance->currentScene;

						float rX = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
						float rY = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
						float rZ = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5

						scene->dim->SpawnItem(selectedBlock->position + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(rX,rY,rZ), item, 10.0f, 1.0f);
					}

					c->chunkMutex.lock();
					c->ModifyBlock(_world.x, _world.y, _world.z, 0);
					c->chunkMutex.unlock();

					selectedBlock = nullptr;
				}
			}

			if (selectedBlock != nullptr)
			{

				std::vector<BlockFace> faces = {};

				faces.push_back(selectedBlock->BreakTopFace());
				ApplyNormal(faces[0].vertices, glm::vec3(0, 1, 0));
				faces.push_back(selectedBlock->BreakBottomFace());
				ApplyNormal(faces[1].vertices, glm::vec3(0, -1, 0));
				faces.push_back(selectedBlock->BreakLeftFace());
				ApplyNormal(faces[2].vertices, glm::vec3(1, 0, 0));
				faces.push_back(selectedBlock->BreakRightFace());
				ApplyNormal(faces[3].vertices, glm::vec3(-1, 0, 0));
				faces.push_back(selectedBlock->BreakFrontFace());
				ApplyNormal(faces[4].vertices, glm::vec3(0, 0, -1));
				faces.push_back(selectedBlock->BreakBackFace());
				ApplyNormal(faces[5].vertices, glm::vec3(0, 0, 1));

				for (int i = 0; i < faces.size(); i++)
				{
					BlockFace f = faces[i];

					for (int j = 0; j < f.vertices.size(); j++)
						f.vertices[j].position += f.vertices[j].normal * 0.03f;

					DrawBlockBreak(f);
				}

				RenderBreak();
			}
		}
		else if (selectedBlock != nullptr)
		{
			selectedBlock->breakProgress = 0;
		}
	}

	if (!freeCam)
		Entity::Draw(); // physics
}

void Player::MouseClick(int button, glm::vec2 mPos)
{
	if (_inInventory && !Hud::GamePaused)
		return;

	Camera* camera = Game::instance->GetCamera();

	if (button == GLFW_MOUSE_BUTTON_RIGHT && selectedBlock != nullptr)
	{
		if (selectedBlock->isInteractable)
		{
			selectedBlock->OnInteract(); 
			return;
		}

		glm::vec3 ray = position + (camera->cameraFront * 5.0f);
		bool hit = RayTo(ray);
		if (hit)
		{
			float x = ray.x;
			int y = (int)ray.y;
			float z = ray.z;

			Chunk* c = WorldManager::instance->GetChunk(x, z);

			int type = c->GetBlock(x, y, z);

			if (type > 0 && type != WATER)
				return;

			if ((int)x == (int)position.x && (int)z == (int)position.z && (y == (int)position.y || y == (int)position.y - 1))
				return;

			if (c != nullptr)
			{
				Gameplay* scene = (Gameplay*)Game::instance->currentScene;

				int selected = scene->hud->selected;

				Data::InventoryItem item = playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1];

				if (item.type != Data::ITEM_NULL && item.placeable)
				{

					Data::BlockData d = Data::BlockData();
					if (item.type == Data::ITEM_TORCH)
					{
						if (selectedBlock->type == TORCH) // can't place torch on torch
							return;

						if (selectedFace.vertices.size() != 0)
						{
							if (selectedFace.vertices[0].normal == glm::vec3(0, 1, 0))
								d.AddTag("facing", "0");
							else if (selectedFace.vertices[0].normal == glm::vec3(0, -1, 0))
								return;
							else if (selectedFace.vertices[0].normal == glm::vec3(1, 0, 0))
								d.AddTag("facing", "1");
							else if (selectedFace.vertices[0].normal == glm::vec3(-1, 0, 0))
								d.AddTag("facing", "2");
							else if (selectedFace.vertices[0].normal == glm::vec3(0, 0, -1))
								d.AddTag("facing", "3");
							else if (selectedFace.vertices[0].normal == glm::vec3(0, 0, 1))
								d.AddTag("facing", "4");
						}
						else
							d.AddTag("facing", "0");
					}

					Block* b = c->CreateBlock(x, y, z, item.type, d);
					c->PlaceBlock(x, y, z, b);

					if (item.count == 1)
						playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1] = {};
					else
						playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1].count--;

					scene->hud->UpdateHotbar();
				}
				
			}
		}
	}
}

void Player::KeyPress(int key)
{
	Gameplay* scene = (Gameplay*)Game::instance->currentScene;
	Camera* c = Game::instance->GetCamera();
	Data::InventoryItem item;
	int selected = 0;

	if (key == GLFW_KEY_F1) // place light
		LightingManager::GetInstance()->AddLight(glm::vec3((int)position.x, (int)position.y + 2, (int)position.z), 12);

	if (key == GLFW_KEY_F2) // remove light
		LightingManager::GetInstance()->RemoveLight(glm::vec3((int)position.x, (int)position.y + 2, (int)position.z));

	// select hotbar

	switch (key)
	{
	case GLFW_KEY_1:
		scene->hud->SetSelected(0);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_2:
		scene->hud->SetSelected(1);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_3:
		scene->hud->SetSelected(2);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_4:
		scene->hud->SetSelected(3);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_5:
		scene->hud->SetSelected(4);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_6:
		scene->hud->SetSelected(5);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_7:
		scene->hud->SetSelected(6);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_8:
		scene->hud->SetSelected(7);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_9:
		scene->hud->SetSelected(8);
		scene->hud->UpdateHotbar();
		break;
	case GLFW_KEY_Q:
		selected = scene->hud->selected;

		item = playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1];

		if (item.type != Data::ITEM_NULL)
		{
			if (item.count == 1)
			{
				scene->dim->SpawnItem(position + c->cameraFront, c->cameraFront, item);
				playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1] = {};
			}
			else
			{
				item.count--;
				playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1] = item;
				item.count = 1;
				scene->dim->SpawnItem(position + c->cameraFront, c->cameraFront, item);
			}

			scene->hud->UpdateHotbar();
		}
		break;
	case GLFW_KEY_E:
		if (!_inInventory && !Hud::GamePaused)
			ToggleInventory();
		break;
	case GLFW_KEY_ESCAPE:
		if (_inInventory)
			ToggleInventory();
		else
			TogglePauseMenu();
		break;
	case GLFW_KEY_H:
		instantBreak = !instantBreak;
		break;
	}

}

Player::~Player()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}