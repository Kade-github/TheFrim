#include "Spritesheet.h"

#include <pugixml.hpp>

void SpriteSheet::Load(const char* filename, int width, int height)
{
	sprites.clear();

	pugi::xml_document doc;
	doc.load_file(filename);

	pugi::xml_node root = doc.child("TextureAtlas");

	this->width = width;
	this->height = height;

	for (pugi::xml_node node = root.child("sprite"); node; node = node.next_sibling("sprite"))
	{
		Sprite sprite;

		sprite.name = node.attribute("n").as_string();
		sprite.x = node.attribute("x").as_int();
		sprite.y = node.attribute("y").as_int();
		sprite.width = node.attribute("w").as_int();
		sprite.height = node.attribute("h").as_int();

		sprites.push_back(sprite);
	}

}

const Sprite SpriteSheet::GetSprite(std::string s) const
{
	for (auto& sprite : sprites)
	{
		if (sprite.name == s)
		{
			return sprite;
		}
	}

	return Sprite();
}

glm::vec4 SpriteSheet::GetUV(std::string s) const
{
	Sprite sprite = GetSprite(s);

	if (sprite.name == "")
	{
		return glm::vec4(0, 0, 0, 0);
	}

	float x = (float)sprite.x / (float)width;
	float y = (float)sprite.y / (float)height;

	float w = (float)sprite.width / (float)width;
	float h = (float)sprite.height / (float)height;

	return glm::vec4(x, y, w, h);
}

glm::vec4 SpriteSheet::GetUVFlip(std::string s) const
{
	Sprite sprite = GetSprite(s);

	if (sprite.name == "")
	{
		return glm::vec4(0, 0, 0, 0);
	}

	float x = (float)sprite.x / (float)width;
	float y = (float)sprite.y / (float)height;

	float w = (float)sprite.width / (float)width;
	float h = (float)sprite.height / (float)height;

	return glm::vec4(x + w, y, -w, h);
}
