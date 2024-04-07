#include "BoxUI.h"

BoxUI::BoxUI(glm::vec3 _pos, int _w, int _h) : GameObject2D(_pos)
{
	width = _w;
	height = _h;

	if (width < 2)
		width = 2;

	if (height < 2)
		height = 2;

	t = Texture::createWithImage("Assets/Textures/ui_sheet.png", false);
	t->spriteSheet.Load("Assets/Textures/ui_sheet.xml", t->width, t->height);

	SetBox();
}

BoxUI::~BoxUI()
{
	for (int i = 0; i < back.size(); i++)
		delete back[i];
	back.clear();
}

void BoxUI::SetBox()
{
	for (int i = 0; i < back.size(); i++)
		delete back[i];

	back.clear();

	for (int i = 0; i < slots.size(); i++)
		delete slots[i];

	slots.clear();

	for (int i = 0; i < front.size(); i++)
		delete front[i];

	front.clear();

	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			std::string tag = "space";

			if (x == 0)
				tag = "left";

			if (x == width - 1)
				tag = "right";

			if (y == height - 1)
				tag = "top";

			if (y == 0)
				tag = "bottom";

			if (x == 0 && y == 0)
				tag = "corner_bottomleft";

			if (x == 0 && y == height - 1)
				tag = "corner_topleft";

			if (x == width - 1 && y == 0)
				tag = "corner_bottomright";

			if (x == width - 1 && y == height - 1)
				tag = "corner_topright";


			Sprite2D* s = new Sprite2D(t, glm::vec3(0, 0, 0));
			s->width = 128;
			s->height = 128;

			s->src = t->spriteSheet.GetUVFlip(tag);

			back.push_back(s);

			if (tag == "space")
			{
				Sprite2D* slot = new Sprite2D(t, glm::vec3(0, 0, 0));

				slot->width = 128;
				slot->height = 128;

				slot->src = t->spriteSheet.GetUVFlip("box_slot");
				slot->tag_id = std::to_string(x) + "," + std::to_string(y);

				slot->position = position + glm::vec3(slot->width * x, slot->height * y, 0);

				slots.push_back(slot);
			}
		}
	}

	renderWidth = width * 128;
	renderHeight = height * 128;
}

Sprite2D* BoxUI::GetFront(glm::vec2 _closestPos)
{
	// check if the mouse is inside the box

	Sprite2D* sp = nullptr;

	for (Sprite2D* s : front)
	{
		if (_closestPos.x >= s->position.x && _closestPos.x <= s->position.x + s->width &&
			_closestPos.y >= s->position.y && _closestPos.y <= s->position.y + s->height)
			sp = s;
	}

	return sp;
}

Sprite2D* BoxUI::GetSlot(glm::vec2 _closestPos)
{
	// check if the mouse is inside the box

	Sprite2D* sp = nullptr;

	for (Sprite2D* s : slots)
	{
		if (_closestPos.x >= s->position.x && _closestPos.x <= s->position.x + s->width &&
			_closestPos.y >= s->position.y && _closestPos.y <= s->position.y + s->height)
			sp = s;
	}

	return sp;
}

void BoxUI::AddFront(Sprite2D* s, int x, int y)
{
	front.push_back(s);

	s->width = 128;
	s->height = 128;

	s->position = position + glm::vec3(s->width * x, s->height * y, 0);
}

void BoxUI::RemoveFront(Sprite2D* s)
{
	for (int i = 0; i < front.size(); i++)
	{
		if (front[i]->id == s->id)
		{
			delete front[i];
			front.erase(front.begin() + i);
			break;
		}
	}
}

void BoxUI::ClearFront()
{
	for(Sprite2D* s : front)
		delete s;

	front.clear();
}

void BoxUI::Draw()
{
	draws.clear();
	for (int i = 0; i < back.size(); i++)
	{
		Sprite2D* s = back[i];

		s->position = position + glm::vec3(s->width * (i % width), s->height * (i / width), 0);

		s->Draw();
		draws.push_back(s->draws[0]);
	}

	for (int i = 0; i < slots.size(); i++)
	{
		Sprite2D* s = slots[i];

		s->Draw();
		draws.push_back(s->draws[0]);
	}

	for (int i = 0; i < front.size(); i++)
	{
		Sprite2D* s = front[i];

		s->Draw();
		draws.push_back(s->draws[0]);
	}
}
