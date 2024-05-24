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

void Player::Hurt(float damage, glm::vec3 from)
{
	if (playerData.health <= 0)
		return;

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

void Player::ToggleFurnace()
{
	_inInventory = !_inInventory;

	if (!_inInventory)
		firstMouse = true;

	Gameplay* scene = (Gameplay*)Game::instance->currentScene;

	scene->hud->ShowFurnace(_inInventory);
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
	speed = 0.4f;
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

	bool wasShift = shift;

	if (isOnGround && !_inInventory)
		shift = glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;

	if (shiftCancel && shift)
	{
		if (!isOnGround)
			shift = false;
		else
			shiftCancel = false;
	}

	if (!wasShift)
		blockOnShift.y = -1000;

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

	bool hasLandedThisFrame = false;

	float ourForward = 0;
	float ourStrafe = 0;

	if (!freeCam)
	{
		if (!_inInventory && !Hud::GamePaused && !Hud::endSequence)
		{
			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_W) == GLFW_PRESS)
			{
				ourForward += sp;
				headBop += 10.0f * Game::instance->deltaTime;
				moving = true;
			}

			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_S) == GLFW_PRESS)
			{
				ourForward -= sp;
				headBop += 10.0f * Game::instance->deltaTime;
				moving = true;
			}

			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_A) == GLFW_PRESS)
			{
				ourStrafe -= sp;
				headBop += 10.0f * Game::instance->deltaTime;
				moving = true;
			}

			if (glfwGetKey(Game::instance->GetWindow(), GLFW_KEY_D) == GLFW_PRESS)
			{
				ourStrafe += sp;
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
				hasLandedThisFrame = true;
				isOnGround = false;
			}


			forwardVelocity += ourForward;
			strafeVelocity += ourStrafe;
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
	}

	if ((isOnGround || hasLandedThisFrame) && jumpedFrom > -1)
	{
		float distance = jumpedFrom - position.y;

		float fallDamage = (distance - 2) / 2;

		// round to nearest half or whole

		fallDamage = floor((fallDamage * 2) + 0.5f) / 2;

		if (distance < 4)
			fallDamage = 0;

		// factor in boots

		if (playerData.armor[2].armor > 0)
			fallDamage -= (float)playerData.armor[2].armor / 2.0f;

		if (fallDamage > 0 && !inWater)
		{
			MusicManager::GetInstance()->PlaySFX("fall", "fallSFX");
			Hurt(fallDamage);
		}

		jumpedFrom = -1;
	}
	else if (!(isOnGround || hasLandedThisFrame) && jumpedFrom <= -1)
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

	if (!freeCam && !Hud::endSequence)
	{
		static float timer = 0;
		glm::vec3 storedPos = position - (front * 0.1f);

		glm::vec3 fr = front * 1.1f;
		fr.y = 0.2f;

		glm::vec3 shiftPos = position - glm::vec3(0, 0.15, 0) - fr;

		if (!shift)
		{
			if (timer > 0)
				timer -= 10 * Game::instance->deltaTime;

			float perc = timer / 4.0f;

			camera->position = glm::mix(storedPos, shiftPos, perc);
		}
		else
		{
			if (timer < 1.0f)
				timer += 10 * Game::instance->deltaTime;

			float perc = timer / 4.0f;

			if (perc > 1)
				perc = 1;

			camera->position = glm::mix(storedPos, shiftPos, perc);
		}
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

	if (!Hud::endSequence)
	{

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
		SetDirection();
	}

	glm::vec3 ray = position + (camera->cameraFront * 5.0f);

	AI* selectedEntity = nullptr;

	bool hit = RayToCustom(camera->position, ray, true);
	if (hit)
	{
		//camera->DrawDebugCube(ray - camera->cameraFront, glm::vec3(0.05f));
		Chunk* c = WorldManager::instance->GetChunk(ray.x, ray.z);

		if (c != nullptr && c->isLoaded && !c->isBeingLoaded)
		{
			std::lock_guard<std::mutex> lock(Data::World::worldMutex);
			subChunk& sb = c->GetSubChunk(ray.y);

			if (sb.y != -1)
			{

				glm::vec3 _world = c->WorldToChunk(ray);

				Block* last = selectedBlock;

				selectedBlock = sb.getBlock(_world.x, _world.z);

				if (selectedBlock != nullptr)
				{

					if (last != nullptr && last->breakProgress > selectedBlock->breakProgress)
					{
						last->breakProgress = 0;
						lastBreakSfx = 0;
					}


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
					selectedFace = {};
			}
			else
				selectedBlock = nullptr;

		}
		else
			selectedBlock = nullptr;
	}
	else
	{
		selectedBlock = nullptr;

		Gameplay* gp = (Gameplay*)Game::instance->currentScene;

		std::vector<AI*> mobs = gp->mm->mobs;

		for (AI* mob : mobs)
		{
			if (mob->IsPositionInMe(ray))
			{
				selectedEntity = mob;
				break;
			}
		}

	}


	vertices.clear();
	indices.clear();

	if (!_inInventory && !Hud::GamePaused && !Hud::endSequence)
	{
		if (selectedEntity != nullptr && glfwGetMouseButton(Game::instance->GetWindow(), GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		{
			float damage = playerData.GetInventoryItem(playerData.selectedSlot, PLAYER_INVENTORY_HEIGHT - 1).damage;
			selectedEntity->Hurt(damage, position);
		}

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
				correctTool = true;

				if (StringTools::Contains(item.tag, "axe"))
					toughness *= item.breakingPower;
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

			if (instantBreak)
				giveItem = true;

			switch (selectedBlock->soundType)
			{
			case S_STONE:
				if (correctTool)
				{
					switch (selectedBlock->type)
					{
					case STONE:
					case COBBLESTONE:
						giveItem = item.breakingPower >= 2;
						break;
					case IRON_ORE:
						giveItem = item.breakingPower >= 4;
						break;
					case GOLD_ORE:
						giveItem = item.breakingPower >= 6;
						break;
					case DIAMOND_ORE:
						giveItem = item.breakingPower >= 6;
						break;
					case RUINED_DEBRIS:
						giveItem = item.breakingPower >= 2;
						break;
					}
				}
				break;
			}

			if (!giveItem)
				toughness *= 0.5f;

			if (!correctTool)
				toughness *= 0.5f;

			int p = std::max((int)(selectedBlock->breakProgress * 100), 0);

			if (lastBreakSfx == 0)
			{
				if (std::abs(glfwGetTime() - breakCooldown) > 0.1)
				{
					int rander = (rand() % 1000);
					FootstepSound(selectedBlock, std::to_string(lastBreakSfx + rander), -30);
				}

				lastBreakSfx = 1;
			}

			if (p > 25 && lastBreakSfx == 1)
			{
				int rander = (rand() % 1000);
				FootstepSound(selectedBlock, std::to_string(lastBreakSfx + rander), -20);

				lastBreakSfx = 25;
			}


			if (p > 50 && lastBreakSfx == 25)
			{
				int rander = (rand() % 1000);
				FootstepSound(selectedBlock, std::to_string(lastBreakSfx + rander), -10);

				lastBreakSfx = 50;
			}

			if (p > 75 && lastBreakSfx == 50)
			{
				int rander = (rand() % 1000);
				FootstepSound(selectedBlock, std::to_string(lastBreakSfx + rander), 0);

				lastBreakSfx = 75;
			}

			selectedBlock->breakProgress += (1.0f * Game::instance->deltaTime) * toughness;



			if (selectedBlock->breakProgress >= 1)
			{
				selectedBlock->breakProgress = 0;

				glm::vec3 _world = selectedBlock->position;

				Chunk* c = WorldManager::instance->GetChunk(_world.x, _world.z);

				if (c != nullptr)
				{


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

						scene->dim->SpawnItem(selectedBlock->position + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(rX, rY, rZ), item, 10.0f, 1.0f);
					}

					int rander = (rand() % 1000);
					FootstepSound(selectedBlock, std::to_string(lastBreakSfx + rander), 40);

					lastBreakSfx = 0;

					breakCooldown = glfwGetTime();

					// drop knowledgement tablet (if debris)

					float rander2 = (rand() % 1000);

					if (rander2 > 850) // 20% chance
					{
						if (selectedBlock->type == RUINED_DEBRIS)
						{
							Data::InventoryItem item = { Data::ITEM_KNOWLEDGEMENT_TABLET, 1 };

							item.SetNBT("hint", "-1");

							Gameplay* scene = (Gameplay*)Game::instance->currentScene;

							float rX = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
							float rY = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5
							float rZ = ((rand() % 100) - 50) / 100.0f; // -0.5 to 0.5

							scene->dim->SpawnItem(selectedBlock->position + glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(rX, rY, rZ), item, 10.0f, 1.0f);

						}
					}

					c->ModifyBlock(_world.x, _world.y, _world.z, 0);
					shiftCancel = true;



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
			lastBreakSfx = 0;
		}
	}

	if (!freeCam && !Hud::endSequence)
	{
		Entity::Draw(); // physics
	}
}

void Player::MouseClick(int button, glm::vec2 mPos)
{
	if (_inInventory || Hud::GamePaused || Hud::endSequence)
		return;

	Camera* camera = Game::instance->GetCamera();

	if (button == GLFW_MOUSE_BUTTON_RIGHT) // eat food (or check knowledge tablet)
	{
		// get selected item

		Data::InventoryItem& item = playerData.GetInventoryItem(playerData.selectedSlot, PLAYER_INVENTORY_HEIGHT - 1);

		if (item.edible)
		{
			playerData.health += item.food;

			if (playerData.health > PLAYER_MAX_HEALTH)
				playerData.health = PLAYER_MAX_HEALTH;


			item.count--;

			if (item.count == 0)
				item = {};

			Gameplay* scene = (Gameplay*)Game::instance->currentScene;

			scene->hud->UpdateHearts();
			scene->hud->UpdateHotbar();
		}
		else if (item.type == Data::ITEM_KNOWLEDGEMENT_TABLET)
		{
			Gameplay* scene = (Gameplay*)Game::instance->currentScene;

			scene->hud->ShowKnowledgementTablet(item);
		}
	}

	if (button == GLFW_MOUSE_BUTTON_RIGHT && selectedBlock != nullptr)
	{
		if (selectedBlock->isInteractable && !shift)
		{
			selectedBlock->OnInteract();
			return;
		}
		glm::vec3 ray = position + (camera->cameraFront * 5.0f);
		bool hit = RayToCustom(camera->position, ray);
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

					std::lock_guard<std::mutex> lock(Data::World::worldMutex);

					Block* b = c->CreateBlock(x, y, z, item.type, d);
					c->PlaceBlock(x, y, z, b);

					Gameplay* gp = (Gameplay*)Game::instance->currentScene;

					if (gp->recordBlockData)
					{
						glm::vec3 f = gp->firstBlock;

						if (f.x <= 0 && f.y <= 0 && f.z <= 0)
						{
							gp->firstBlock = { (int)x, (int)y, (int)z };
							f = gp->firstBlock;
						}

						std::string v = std::to_string(f.x - (int)x) + "," + std::to_string(f.y - (int)y) + "," + std::to_string(f.z - (int)z);

						Game::instance->log->Write("Recording " + v + " as " + std::to_string(item.type));

						gp->blockData[v] = item.type;
					}

					FootstepSound(b, "place");

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
		if (!_inInventory)
		{
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
		}
		break;
	case GLFW_KEY_E:
		if (!_inInventory && !Hud::GamePaused)
			ToggleInventory();
		break;
	case GLFW_KEY_ESCAPE:

		if (scene->hud->hintShown)
		{
			scene->c2d->RemoveObject(scene->hud->hint);
			delete scene->hud->hint;

			scene->hud->hintShown = false;
			Hud::GamePaused = false;

			scene->hud->pauseBackground->color = glm::vec4(1, 1, 1, 0);
			scene->hud->resume->color = glm::vec4(1, 1, 1, 0);
			scene->hud->title->color = glm::vec4(1, 1, 1, 0);
			scene->hud->pauseHeader->color = glm::vec4(1, 1, 1, 0);
			return;
		}

		if (_inInventory)
			ToggleInventory();
		else
			TogglePauseMenu();
		break;
	case GLFW_KEY_H:
		//instantBreak = !instantBreak;
		//scene->hud->ShowHint("Instant Break: " + std::to_string(instantBreak));
		break;
	}

}

Player::~Player()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}