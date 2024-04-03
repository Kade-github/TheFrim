#include "Fnt.h"
#include "Game.h"
#include <algorithm>

std::vector<Fnt*>* Fnt::fonts = NULL;

Fnt::Fnt(std::string font)
{
	name = font;
	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(("Assets/Fonts/" + name + ".fnt").c_str());

	if (!result)
	{
		Game::instance->log->Write("Font parsing error: " + std::string(result.description()));
		return;
	}

	pugi::xml_node info = doc.child("font").child("info");

	if (info != NULL)
	{
		ogSize = info.attribute("size").as_int();
	}
	else
	{
		Game::instance->log->Write("Font parsing error: info node not found");
		return;
	}

	pugi::xml_node pages = doc.child("font").child("pages");

	if (pages != NULL)
	{
		std::string page = pages.first_child().attribute("file").as_string();
		std::string path = "Assets/Fonts/" + page;
		_texture = Texture::createWithImage(path, false);
		_texture->dontDelete = true;
	}
	else
	{
		Game::instance->log->Write("Font parsing error: pages node not found");
		return;
	}

	pugi::xml_node chrs = doc.child("font").child("chars");

	if (chrs != NULL)
	{
		for (pugi::xml_node c : chrs.children())
		{
			FntChar fc;
			fc.id = c.attribute("id").as_int();
			float x = c.attribute("x").as_float();
			float y = c.attribute("y").as_float();
			float w = c.attribute("width").as_float();
			float h = c.attribute("height").as_float();

			fc.dst = { x, y, w, h };

			fc.src = { x / _texture->width, y / _texture->height,w / _texture->width, h / _texture->height };

			fc.xAdvance = c.attribute("xadvance").as_int();

			chars.push_back(fc);
		}
	}
	else
	{
		Game::instance->log->Write("Font parsing error: chars node not found");
		return;
	}

	std::sort(chars.begin(), chars.end());

	pugi::xml_node kernings = doc.child("font").child("kernings");

	if (kernings != NULL)
	{
		hasKernings = true;
		for (pugi::xml_node k : kernings.children())
		{
			int id = k.attribute("first").as_int();
			FntChar* fc = GetCharPtr(id);

			if (fc == NULL)
			{
				Game::instance->log->Write("Font parsing error: kerning char for " + std::to_string(id) + " not found!");
				continue;
			}

			Kerning kern;
			kern.with = k.attribute("second").as_int();
			kern.amount = k.attribute("amount").as_int();

			fc->kernings.push_back(kern);
		}
	}
	else
	{
		Game::instance->log->Write("Font parsing error: kernings node not found");
		return;
	}

	Game::instance->log->Write("Font: " + name + " loaded");

}

void Fnt::ClearCache()
{
	if (fonts == NULL)
		fonts = new std::vector<Fnt*>();

	if (fonts->size() == 0)
		return;
	fonts->clear();
}

Fnt* Fnt::GetFont(std::string font)
{
	if (fonts == NULL)
		fonts = new std::vector<Fnt*>();

	for (Fnt* f : *fonts)
		if (f->name == font)
			return f;

	Fnt* f = new Fnt(font);

	fonts->push_back(f);
	return f;
}
