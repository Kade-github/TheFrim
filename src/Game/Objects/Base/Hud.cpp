#include "Hud.h"
#include <Game.h>
#include <Helpers/Collision2D.h>
#include "../../MusicManager.h"
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
}

void Hud::ShowPauseMenu(bool s)
{
	GamePaused = s;

	Game::instance->SetLockedCursor(!s);

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

void Hud::SetSelected(int s)
{
	selected = s;

	Data::InventoryItem& item = player->playerData.inventory[selected][PLAYER_INVENTORY_HEIGHT - 1];

	if (item.type != Data::ItemType::ITEM_NULL)
	{
		hand->src = i->spriteSheet.GetUVFlip(item.tag);
		hand->position.y = -(hand->height / 6);
		hand->flipHorizontal = true;
	}
	else
	{
		hand->src = i->spriteSheet.GetUVFlip("hand");
		hand->position.y = 0;
		hand->flipHorizontal = false;
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

void Hud::UpdateArmor()
{
}

Hud::Hud(glm::vec3 _pos, Player* _p, Camera2D* _c2d) : GameObject(_pos)
{
	player = _p;
	c2d = _c2d;

	h = Texture::createWithImage("Assets/Textures/hud.png", false);
	h->spriteSheet.Load("Assets/Textures/hud.xml", h->width, h->height);
	i = Texture::createWithImage("Assets/Textures/items.png", false);
	i->spriteSheet.Load("Assets/Textures/items.xml", i->width, i->height);

	hand = new Sprite2D(i, glm::vec3(0, 0, 0));

	hand->width = 512;
	hand->height = 512;

	hand->src = i->spriteSheet.GetUVFlip("hand");

	hand->position.x = c2d->_w - hand->width;

	crosshair = new Sprite2D("Assets/Textures/crosshair.png", glm::vec3(c2d->_w / 2, c2d->_h / 2, 0));

	c2d->AddObject(crosshair);
	crosshair->order = 1;

	// create hand

	c2d->AddObject(hand);
	hand->order = -1;

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

	inv = new Inventory(glm::vec3(0,0,0), player);

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
	UpdateArmor();
}

Hud::~Hud()
{
	delete crosshair;
	
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

	armor.clear();

	delete hand;
	delete pauseBackground;
	delete resume;
	delete title;
	delete pauseHeader;
}

void Hud::Draw()
{
	crosshair->position = glm::vec3((c2d->_w / 2) - crosshair->width / 2, (c2d->_h / 2) - crosshair->height / 2, 0);

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

	if (GamePaused)
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
}

void Hud::MouseClick(int button, glm::vec2 pos)
{
	if (GamePaused)
	{
		if (Collision2D::PointInRect(pos, resume->position, glm::vec2(resume->width, resume->height)))
		{
			MusicManager::GetInstance()->PlaySFX("select");
			ShowPauseMenu(false);
		}
		else if (Collision2D::PointInRect(pos, title->position, glm::vec2(title->width, title->height)))
		{
			MusicManager::GetInstance()->PlaySFX("select");
			GamePaused = false;
			WorldManager::instance->SaveWorldNow();
			Game::instance->SwitchScene(new MainMenu());
		}
	}
}
