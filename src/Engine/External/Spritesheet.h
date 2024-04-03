#ifndef _SPRITESHEET_H_
#define _SPRITESHEET_H_

#include <glm/glm.hpp>
#include <vector>

#include <string>

struct Sprite
{
	std::string name = "";
	int x = -1, y = -1, width = -1, height = -1;
};

class SpriteSheet
{
	int width, height;

	std::vector<Sprite> sprites;

public:
	void Load(const char* filename, int width, int height);

	const Sprite GetSprite(std::string s) const;

	glm::vec4 GetUV(std::string s) const;
	glm::vec4 GetUVFlip(std::string s) const;
};

#endif