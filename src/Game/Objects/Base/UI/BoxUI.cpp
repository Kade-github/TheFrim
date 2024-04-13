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
			s->width = 64;
			s->height = 64;


			s->src = t->spriteSheet.GetUVFlip(tag);

			back.push_back(s);

		}
	}

	renderWidth = width * 64;
	renderHeight = height * 64;
}

void BoxUI::ClearFronts()
{
	for (int i = 0; i < boxSlots.size(); i++)
	{
		delete boxSlots[i].front;
		boxSlots[i].front = nullptr;
	}
}

void BoxUI::AddSlot(float x, float y, int id)
{
	BoxSlot slot;
	slot.x = x;
	slot.y = y;
	slot.id = id;

	Sprite2D* s = new Sprite2D(t, glm::vec3(0, 0, 0));
	s->width = 64;
	s->height = 64;

	s->src = t->spriteSheet.GetUVFlip("box_slot");

	s->tag_id = std::to_string(x) + "," + std::to_string(y);

	slot.slot = s;

	boxSlots.push_back(slot);
}

void BoxUI::AddFront(Sprite2D* s, int id)
{
	for (int i = 0; i < boxSlots.size(); i++)
	{
		if (boxSlots[i].id == id)
		{
			s->position = glm::vec3(boxSlots[i].x * 64, boxSlots[i].y * 64, 0);

			boxSlots[i].front = s;
			boxSlots[i].front->width = 32;
			boxSlots[i].front->height = 32;
			return;
		}
	}
}

BoxSlot& BoxUI::GetSlot(glm::vec2 pos)
{
	static BoxSlot nslot;
	nslot.id = -1;
	for (int i = 0; i < boxSlots.size(); i++)
	{
		BoxSlot& slot = boxSlots[i];
		Sprite2D* s = slot.slot;

		glm::vec3 p = position + glm::vec3(s->width * slot.x, s->height * slot.y, 0);

		if (p.x <= pos.x && p.x + s->width > pos.x && p.y <= pos.y && p.y + s->height > pos.y)
			return slot;
	}

	return nslot;
}

BoxSlot& BoxUI::GetSlot(int id)
{
	static BoxSlot nslot;
	nslot.id = -1;
	for (int i = 0; i < boxSlots.size(); i++)
	{
		BoxSlot& slot = boxSlots[i];

		if (slot.id == id)
			return slot;
	}

	return nslot;
}

Sprite2D* BoxUI::GetFront(glm::vec2 pos)
{
	BoxSlot& slot = GetSlot(pos);

	return slot.front;
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

	for (int i = 0; i < boxSlots.size(); i++)
	{
		Sprite2D* s = boxSlots[i].slot;

		s->position = position + glm::vec3(s->width * boxSlots[i].x, s->height * boxSlots[i].y, 0);
		s->Draw();
		draws.push_back(s->draws[0]);
	}

	for (int i = 0; i < boxSlots.size(); i++)
	{
		Sprite2D* f = boxSlots[i].front;
		if (f != nullptr)
		{
			f->position += glm::vec3(16, 16, 0);
			f->Draw();
			f->position -= glm::vec3(16, 16, 0);
			draws.push_back(f->draws[0]);
		}
	}
}
