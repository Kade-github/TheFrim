#include "Hud.h"

void Hud::UpdateHotbar()
{
}

void Hud::UpdateHearts()
{
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
		s->position = glm::vec3((c2d->_w / 2) - (s->width * 4) + (i * s->width), c2d->_h - s->height, 0);
		s->order = 1;
		hotbar.push_back(s);
		c2d->AddObject(s);
	}
}

Hud::~Hud()
{
	delete crosshair;
	delete h;
}

void Hud::Draw()
{
	crosshair->position = glm::vec3((c2d->_w / 2) - crosshair->width / 2, (c2d->_h / 2) - crosshair->height / 2, 0);
}
