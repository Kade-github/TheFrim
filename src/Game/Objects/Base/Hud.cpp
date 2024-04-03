#include "Hud.h"
#include <Game.h>

void Hud::SetSelected(int s)
{
	selected = s;
	UpdateHotbar();
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

		Sprite2D* s = new Sprite2D(i, glm::vec3(0, 0, 0));

		s->width = 64;
		s->height = 64;




	}


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
			s->src = h->spriteSheet.GetUV("hud_heart_half");
		else if (hpProgress <= rI) // empty heart
			s->src = h->spriteSheet.GetUV("hud_heart_empty");
		else // full heart
			s->src = h->spriteSheet.GetUV("hud_heart");

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

	h = Texture::createWithImage("Assets/Textures/hud.png");
	h->spriteSheet.Load("Assets/Textures/hud.xml", h->width, h->height);
	i = Texture::createWithImage("Assets/Textures/items.png");
	i->spriteSheet.Load("Assets/Textures/items.xml", i->width, i->height);

	crosshair = new Sprite2D("Assets/Textures/crosshair.png", glm::vec3(c2d->_w / 2, c2d->_h / 2, 0));

	c2d->AddObject(crosshair);
	crosshair->order = 1;

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

	UpdateHotbar();
	UpdateHearts();
	UpdateArmor();
}

Hud::~Hud()
{
	delete crosshair;
	delete h;
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
}
