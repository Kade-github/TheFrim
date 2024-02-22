#ifndef _FNT_H
#define _FNT_H

#include <vector>
#include <pugixml.hpp>
#include <glm/glm.hpp>
#include <OpenGL/Texture.h>

struct Kerning {
	int with = 0;
	int amount = 0;
};

struct FntChar
{
	glm::vec4 dst;
	glm::vec4 src;
	int id = -1;
	int xAdvance = 0;
	std::vector<Kerning> kernings;

	bool operator<(const FntChar& a) const
	{
		return id < a.id;
	}
};

class Fnt {
	Texture* _texture = NULL;
	std::string _path = "";
public:
	int ogSize = 0;
	static std::vector<Fnt*>* fonts;
	bool hasKernings = false;
	std::vector<FntChar> chars{};
	std::string name = "";

	Fnt(std::string font);

	static void ClearCache();

	static Fnt* GetFont(std::string font);

	Texture* GetTexture()
	{
		return _texture;
	}

	FntChar GetChar(char c)
	{
		unsigned int u = c;

		auto it = std::find_if(chars.begin(), chars.end(), [&u](const FntChar& obj) {return obj.id == u; });

		if (it != chars.end())
			return *it;

		FntChar cc;
		cc.id = -1;

		return cc;
	}

	FntChar* GetCharPtr(char c)
	{
		unsigned int u = c;

		for (FntChar& cc : chars)
			if (cc.id == u)
				return &cc;

		return NULL;
	}
};

#endif