#include "Hud.h"
#include <Game.h>

glm::vec4 Hud::GetHudSrc(float x, float y)
{
	return glm::vec4((x * 128.0f) / h->width, (y * 128.0f) / h->height, 128.0f / h->width, 128.0f / h->height);
}

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
		s->src = GetHudSrc(0, 1);
	}

	Sprite2D* s = hotbar[selected];

	s->src = GetHudSrc(0, 0);

	// hotbar items

	for (auto h : hotbarItems)
	{
		c2d->RemoveObject(h);
		delete h;
	}

	hotbarItems.clear();



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

	for (int i = 0; i < 10; i++)
	{
		Sprite2D* s = new Sprite2D(h, glm::vec3(0, 0, 0));

		s->width = 72;
		s->height = 72;

		s->position = glm::vec3(((c2d->_w - 162) / 2) - (i * (s->width / 2)), s->height + 72, 0);

		float rI = 9.0f - (float)i;
		
		if (hpProgress <= rI + 0.5f && hpProgress >= rI + 0.1f) // half heart
			s->src = GetHudSrc(0, 2);
		else if (hpProgress <= rI) // empty heart
			s->src = GetHudSrc(0, 3);
		else // full heart
			s->src = GetHudSrc(0, 4);

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
		s->src = GetHudSrc(0, 1);
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
