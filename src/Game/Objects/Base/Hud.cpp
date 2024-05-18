#include "Hud.h"
#include <Game.h>
#include <Helpers/Collision2D.h>
#include "../../MusicManager.h"
#include "../../LightingManager.h"
#include "../../WorldManager.h"

#include "../../Scenes/MainMenu.h"

bool Hud::GamePaused = false;

void Hud::InventoryShown(bool s)
{
	Game::instance->SetLockedCursor(!s);

	if (!s)
		inv->Close();
	else
	{
		inv->CreateInventory();
		inv->CreateArmor();
		inv->CreateTwoByTwoCrafting();
	}

	inv->shown = s;
	inv->UpdateInventory();
	GamePaused = false;
}

void Hud::ShowCraftingTable(bool s)
{
	if (inv->shown && s)
		return;

	Game::instance->SetLockedCursor(!s);

	if (!s)
		inv->Close();
	else
	{
		inv->CreateInventory();
		inv->CreateThreeByThreeCrafting();
	}

	inv->shown = s;
	inv->UpdateInventory();
	GamePaused = false;
}

void Hud::ShowFurnace(bool s)
{
	if (inv->shown && s)
		return;

	Game::instance->SetLockedCursor(!s);

	if (!s)
		inv->Close();
	else
	{
		inv->CreateInventory();
		inv->CreateFurnace();
	}

	inv->shown = s;
	inv->UpdateInventory();
	GamePaused = false;
}

void Hud::ShowPauseMenu(bool s)
{
	GamePaused = s;

	Game::instance->SetLockedCursor(!s);

	pauseHeader->text = "Game Paused";

	resume->SetText("Resume");

	if (s)
	{
		pauseBackground->color = glm::vec4(1, 1, 1, 0.5);
		resume->color = glm::vec4(1, 1, 1, 1);
		title->color = glm::vec4(1, 1, 1, 1);
		pauseHeader->color = glm::vec4(1, 1, 1, 1);
	}
	else
	{
		pauseBackground->color = glm::vec4(1, 1, 1, 0);
		resume->color = glm::vec4(1, 1, 1, 0);
		title->color = glm::vec4(1, 1, 1, 0);
		pauseHeader->color = glm::vec4(1, 1, 1, 0);
	}
}

void Hud::ShowDeathScreen()
{
	GamePaused = true;

	Game::instance->SetLockedCursor(false);

	resume->color = glm::vec4(1, 1, 1, 1);
	title->color = glm::vec4(1, 1, 1, 1);
	pauseHeader->color = glm::vec4(1, 1, 1, 1);

	pauseHeader->text = "You Died!";

	// show death overlay

	deathOverlay->color.a = 0.5f;

	// change text for resume

	resume->SetText("Respawn");
}

void Hud::ShowKnowledgementTablet(Data::InventoryItem& item)
{
	if (hintShown || GamePaused)
		return;

	std::string level = item.GetNBT("hint").value;
	int hintLevel = -1;
	if (!level.empty())
		hintLevel = std::stoi(level);

	if (hintLevel == -1)
	{
		hintLevel = WorldManager::instance->_world.hintLevel;
		WorldManager::instance->_world.hintLevel++;
		if (WorldManager::instance->_world.hintLevel > 3)
			WorldManager::instance->_world.hintLevel = 0;
		item.SetNBT("hint", std::to_string(hintLevel));
	}

	std::string hintName = "reinf_hint";

	switch (hintLevel)
	{
	case 3:
		hintName = "rckt_hint";
		break;
	case 2:
		hintName = "rckteng_hint";
		break;
	case 1:
		hintName = "condes_hint";
		break;
	default:
		hintName = "reinf_hint";
		break;
	}


	hint = new Sprite2D("Assets/Textures/" + hintName + ".png", glm::vec3(0, 0, 0));
	hint->width = c2d->_w;
	hint->height = c2d->_h;

	hint->order = 2;

	c2d->AddObject(hint);

	GamePaused = true;

	hintShown = true;
}

void Hud::SetSelected(int s)
{
	selected = s;
	player->playerData.selectedSlot = selected;

	Data::InventoryItem& item = player->playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1];

	if (item.type != Data::ItemType::ITEM_NULL)
	{
		hand->src = i->spriteSheet.GetUVFlip(item.tag);
		hand->UpdateSprite();
	}
	else
	{
		hand->src = {};
		hand->UpdateSprite();
	}
}

void Hud::UpdateHotbar()
{
	for (auto h : hotbar)
	{
		Sprite2D* s = h;
		s->src = this->h->spriteSheet.GetUV("hud_slot");
	}

	Sprite2D* s = hotbar[selected];

	s->src = h->spriteSheet.GetUV("hud_slot_selected");

	// hotbar items

	for (auto h : hotbarItems)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	hotbarItems.clear();

	int y = PLAYER_INVENTORY_HEIGHT - 1; // bottom row

	for (int x = 0; x < PLAYER_INVENTORY_WIDTH; x++)
	{
		Data::InventoryItem& item = player->playerData.inventory[x][y];

		if (item.type == Data::ItemType::ITEM_NULL)
			continue;

		ItemUI* s = new ItemUI(item.tag, glm::vec3(0, 0, 0), i, item.count);

		s->width = 48;
		s->height = 48;

		s->position = glm::vec3(((c2d->_w / 2) - (384) + (x * 96)) + 24, 56, 0);

		s->order = 1;
		hotbarItems.push_back(s);
		c2d->AddObject(s);


	}

	inv->UpdateInventory();
	UpdateArmor();

	SetSelected(selected);
}

void Hud::UpdateHearts()
{
	for (auto h : hearts)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	hearts.clear();

	float hpProgress = player->playerData.health;

	for (int i = 0; i < (int)(PLAYER_MAX_HEALTH); i++)
	{
		Sprite2D* s = new Sprite2D(h, glm::vec3(0, 0, 0));

		s->width = 72;
		s->height = 72;

		s->position = glm::vec3(((c2d->_w - 162) / 2) - (i * (s->width / 2)), s->height + 72, 0);

		float rI = 9.0f - (float)i;

		if (hpProgress <= rI + 0.5f && hpProgress >= rI + 0.1f) // half heart
			s->src = h->spriteSheet.GetUVFlip("hud_heart_half");
		else if (hpProgress <= rI) // empty heart
			s->src = h->spriteSheet.GetUVFlip("hud_heart_empty");
		else // full heart
			s->src = h->spriteSheet.GetUVFlip("hud_heart");

		s->order = 1;
		hearts.push_back(s);
		c2d->AddObject(s);
	}

	_heartUpdate = glfwGetTime();
}

void Hud::ClearAir()
{
	for (auto h : air)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	air.clear();
}

void Hud::UpdateAir()
{
	ClearAir();

	float airProgress = player->playerData.air;

	for (int i = 0; i < (int)(PLAYER_MAX_AIR); i++)
	{
		Sprite2D* s = new Sprite2D(h, glm::vec3(0, 0, 0));

		s->width = 72;
		s->height = 72;

		s->position = glm::vec3(((c2d->_w - 162) / 2) - (i * (s->width / 2)), s->height + 144, 0);

		float rI = 9.0f - (float)i;

		if (airProgress <= rI) // empty heart
			s->src = h->spriteSheet.GetUVFlip("hud_air_empty");
		else // full heart
			s->src = h->spriteSheet.GetUVFlip("hud_air_full");

		s->order = 1;
		air.push_back(s);
		c2d->AddObject(s);
	}
}

void Hud::UpdateArmor()
{
	for (auto h : armor)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	armor.clear();

	int armorProgress = 0;

	armorProgress = player->playerData.armor[0].armor + player->playerData.armor[1].armor + player->playerData.armor[2].armor;

	float lastHotbarX = hotbar.back()->position.x + 32;
	int realI = 0;
	int up = 0;
	for (int i = 0; i < armorProgress; i++)
	{
		Sprite2D* s = new Sprite2D(h, glm::vec3(0, 0, 0));

		s->width = 72;
		s->height = 72;

		int rI = realI;

		s->position = glm::vec3(lastHotbarX - (realI * ((s->width + 12) / 2)), s->height + 72, 0);

		for (int j = 0; j < up; j++)
			s->position.y += 36;

		realI = rI;

		while (rI % 8 == 0 && rI != 0)
		{
			rI -= 8;
			realI = rI;

			s->position.y += 36;
			s->position.x = lastHotbarX - (rI * ((s->width + 12) / 2));
			up++;

		}


		s->src = h->spriteSheet.GetUVFlip("hud_armor");

		s->order = 0;
		armor.push_back(s);
		c2d->AddObject(s);
		realI++;
	}
}

Hud::Hud(glm::vec3 _pos, Player* _p, Camera2D* _c2d) : GameObject(_pos)
{
	player = _p;
	c2d = _c2d;

	selected = player->playerData.selectedSlot;

	h = Texture::createWithImage("Assets/Textures/hud.png", false);
	h->spriteSheet.Load("Assets/Textures/hud.xml", h->width, h->height);
	i = Texture::createWithImage("Assets/Textures/items.png", false);
	i->spriteSheet.Load("Assets/Textures/items.xml", i->width, i->height);

	Texture* hn = Texture::createWithImageExtra("Assets/Textures/items.png", "nonFlipped"); // grab from cache
	hn->spriteSheet.Load("Assets/Textures/items.xml", hn->width, hn->height);

	waterOverlay = new Sprite2D("Assets/Textures/waterOverlay.png", glm::vec3(0, 0, 0));

	waterOverlay->width = c2d->_w;
	waterOverlay->height = c2d->_h;

	deathOverlay = new Sprite2D("Assets/Textures/deadOverlay.png", glm::vec3(0, 0, 0));

	deathOverlay->width = c2d->_w;
	deathOverlay->height = c2d->_h;

	deathOverlay->color.a = 0;

	hand = new Sprite3D(hn, glm::vec3(0, 0, 0));
	hand->flipHorizontal = true;
	hand->depth = false;
	hand->width = 0.3;
	hand->height = 0.3;

	hand->src = i->spriteSheet.GetUVFlip("hand");

	hand->UpdateSprite();

	hand->rotateAxis = glm::vec3(0, 1, 0);

	hand->angle = 90;

	hand->position.x = c2d->_w - hand->width;

	crosshair = new Sprite2D("Assets/Textures/crosshair.png", glm::vec3(c2d->_w / 2, c2d->_h / 2, 0));

	c2d->AddObject(waterOverlay);
	c2d->AddObject(deathOverlay);
	waterOverlay->order = 1;
	waterOverlay->color.a = 0;

	c2d->AddObject(crosshair);
	crosshair->order = 1;

	// create hand

	hand->order = 99999;

	// create hotbar (9)

	for (int i = 0; i < 9; i++)
	{
		Sprite2D* s = new Sprite2D(h, glm::vec3(0, 0, 0));

		s->width = 96;
		s->height = 96;

		s->position = glm::vec3((c2d->_w / 2) - (s->width * 4) + (i * s->width), s->height - 64, 0);
		s->src = h->spriteSheet.GetUV("hud_slot");
		s->order = 1;
		hotbar.push_back(s);
		c2d->AddObject(s);
	}

	inv = new Inventory(glm::vec3(0, 0, 0), player);

	inv->position = glm::vec3((c2d->_w / 2) - (inv->renderWidth / 2.25), c2d->_h - (inv->renderHeight * 1.15), 0);

	inv->SetBox();

	c2d->AddObject(inv);

	// create pause stuff

	pauseBackground = new Sprite2D("Assets/Textures/pauseBackground.png", glm::vec3(0, 0, 0));

	pauseBackground->width = c2d->_w;
	pauseBackground->height = c2d->_h;

	pauseBackground->position = glm::vec3(0, 0, 0);
	pauseBackground->color = glm::vec4(1, 1, 1, 0);

	pauseBackground->order = 2;

	c2d->AddObject(pauseBackground);

	resume = new Bar(glm::vec3(0, 0, 0), "Resume Game");
	resume->Resize((resume->width * 0.8) - 100, resume->height * 0.8);

	resume->position.x = (c2d->_w / 2) - (resume->width / 2);
	resume->position.y = (c2d->_h / 2) - (resume->height / 2);
	resume->color = glm::vec4(1, 1, 1, 0);

	resume->order = 3;

	c2d->AddObject(resume);

	title = new Bar(glm::vec3(0, 0, 0), "Save and Quit");
	title->Resize((title->width * 0.8) - 100, title->height * 0.8);

	title->position.x = (c2d->_w / 2) - (title->width / 2);
	title->position.y = (c2d->_h / 2) - (title->height / 2) - 100;
	title->color = glm::vec4(1, 1, 1, 0);

	title->order = 3;

	c2d->AddObject(title);

	pauseHeader = new Text2D("Game Paused", "ArialFrim", glm::vec3(0, 0, 0), glm::vec4(1, 1, 1, 1), 62);

	pauseHeader->position.x = (c2d->_w / 2) - (pauseHeader->width / 2);
	pauseHeader->position.y = resume->position.y + resume->height + 100;
	pauseHeader->color = glm::vec4(1, 1, 1, 0);

	pauseHeader->order = 3;

	c2d->AddObject(pauseHeader);

	UpdateHotbar();
	UpdateHearts();

	hand->position = player->position;
}

Hud::~Hud()
{
	for (auto h : hotbar)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	hotbar.clear();

	for (auto h : hotbarItems)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	hotbarItems.clear();

	for (auto h : hearts)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	hearts.clear();

	for (auto h : armor)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	ClearAir();

	armor.clear();

	delete hand;
}

void Hud::Draw()
{
	if (player->topWater)
	{
		if (waterOverlay->color.a < 0.7f)
			waterOverlay->color.a += 10.0f * Game::instance->deltaTime;
	}
	else
	{
		if (waterOverlay->color.a > 0.0f)
			waterOverlay->color.a -= 10.0f * Game::instance->deltaTime;
	}

	if (GamePaused)
	{
		pauseHeader->position.x = (c2d->_w / 2) - (pauseHeader->width / 2);
		pauseHeader->position.y = resume->position.y + resume->height + 100;
	}

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", player->lightLevel);

	Game::instance->shader->Unbind();

	hand->Draw();

	Game::instance->shader->Bind();

	Game::instance->shader->SetUniform1f("lightLevel", 10.0f);

	Game::instance->shader->Unbind();

	crosshair->position = glm::vec3((c2d->_w / 2) - crosshair->width / 2, (c2d->_h / 2) - crosshair->height / 2, 0);

	Camera* cam = Game::instance->GetCamera();

	// rotate hand according to camera

	glm::vec3 camPos = cam->position + cam->cameraFront;

	glm::vec3 dir = glm::normalize(camPos - player->position);

	float angle = glm::degrees(atan2(dir.z, dir.x));

	hand->angleY = -angle + 90;

	glm::vec3 camRight = glm::normalize(glm::cross(cam->cameraFront, glm::vec3(0, 1, 0)));

	// update hand position

	glm::vec3 finalPos = ((player->position + cam->cameraFront * 0.4f) - glm::vec3(0, 0.4, 0)) + (camRight * 0.25f);

	glm::vec3 lerped = glm::vec3(std::lerp(hand->position.x, finalPos.x, 0.2f), std::lerp(hand->position.y, finalPos.y, 0.2f), std::lerp(hand->position.z, finalPos.z, 0.2f));

	hand->position = lerped;


	// heartupdate tween

	if (_heartUpdate + 0.5f > glfwGetTime())
	{
		float progress = ((_heartUpdate + 0.5f) - glfwGetTime()) / 0.5f;
		for (auto h : hearts)
		{
			h->color.w = std::lerp(1.0f, 0.45f, progress);
		}

	}

	// highlights for pause menu

	glm::vec2 mouse = Game::instance->GetCursorPos();

	if (GamePaused && !inv->shown)
	{
		if (Collision2D::PointInRect(mouse, resume->position, glm::vec2(resume->width, resume->height)))
			resume->color = glm::vec4(1, 1, 1, 0.85);
		else
			resume->color = glm::vec4(1, 1, 1, 1);

		if (Collision2D::PointInRect(mouse, title->position, glm::vec2(title->width, title->height)))
			title->color = glm::vec4(1, 1, 1, 0.85);
		else
			title->color = glm::vec4(1, 1, 1, 1);
	}

	if (_exiting && !Game::instance->DidTakeScreenshot() && !_askedForScreenshot)
	{
		_askedForScreenshot = true;
		// take screenshot

		Game::instance->TakeScreenshot(WorldManager::instance->_path + "/screenshot.png");
	}
	else if (_exiting && _askedForScreenshot)
	{
		// exit

		MainMenu* m = new MainMenu();
		Game::instance->SwitchScene(m);
	}
}

void Hud::MouseClick(int button, glm::vec2 pos)
{
	if (GamePaused && !inv->shown)
	{
		if (Collision2D::PointInRect(pos, resume->position, glm::vec2(resume->width, resume->height)))
		{
			if (deathOverlay->color.a < 0.5f)
			{
				MusicManager::GetInstance()->PlaySFX("select");
				ShowPauseMenu(false);
			}
			else
			{
				player->wasDead = true;
				player->dead = false;
				deathOverlay->color.a = 0.0f;
				ShowPauseMenu(false);
			}
		}
		else if (Collision2D::PointInRect(pos, title->position, glm::vec2(title->width, title->height)))
		{
			MusicManager::GetInstance()->PlaySFX("select");
			player->TogglePauseMenu();

			WorldManager::instance->SetPlayerPosition(player->position);
			WorldManager::instance->SaveWorldNow();
			_exiting = true;
		}
	}

	inv->MouseClick(button, pos);
}
