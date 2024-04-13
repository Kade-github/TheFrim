#include "ItemUI.h"

ItemUI::ItemUI(std::string tag, glm::vec3 _pos, Texture* _t, int _count) : Sprite2D(_t, _pos)
{
	this->tag = tag;

	SetImage();
	SetCount(_count);
}

ItemUI::~ItemUI()
{
	if (countText != NULL)
		delete countText;
}

void ItemUI::SetImage()
{
	src = t->spriteSheet.GetUVFlip(tag);
}

void ItemUI::SetCount(int _count)
{
	count = _count;

	if (count > 1)
	{
		countText = new Text2D(std::to_string(count), "ArialFrim", position, glm::vec4(1, 1, 1, 1), 32.0f);
		countText->position = glm::vec3(position.x + width - countText->width, position.y + countText->height, 0);
	}
	else if (countText != NULL)
	{
		delete countText;
		countText = NULL;
	}
}

void ItemUI::Draw()
{
	draws.clear();
	Sprite2D::Draw();

	if (countText != NULL)
	{
		float sizeDiff = width / 48.0f;

		countText->size = 32.0f * sizeDiff;
		countText->position = glm::vec3(position.x + (width - (countText->width / 2)), position.y - (countText->height / 2), 0);
		countText->Draw();
		draws.push_back(countText->draws[0]);
	}
}
