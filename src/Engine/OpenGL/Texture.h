#pragma once

#ifndef _TEXTURE_H
#define _TEXTURE_H


#include <iostream>
#include <string>
#include "../External/Spritesheet.h"
#include "../../../Include/glm/vec4.hpp"

struct texData
{
	unsigned char* data;
	int w, h;
};

/**
 * \brief The OpenGL Texture Class
 */
class Texture
{
public:
	//GL object
	unsigned int id{};

	std::string path;
	std::string type;

	// Spritesheet

	SpriteSheet spriteSheet;

	//Constructors and destructor

	bool dontDelete = false;

	static Texture* returnWhiteTexture()
	{
		static Texture* t = NULL;
		if (t == NULL)
		{
			unsigned char c[] = { 255, 255, 255, 255 };
			t = new Texture(c, 1, 1);
			t->dontDelete = true;
		}
		return t;
	}

	static Texture* createWithImage(std::string filePath, bool flipped = true);

	static Texture* loadTextureFromData(char* data, size_t outLength);
	static Texture* loadTextureFromData(unsigned char* data, int w, int h);

	static texData getTextureData(std::string filePath);

	void resizeTexture(int w, int h);

	int width{};
	int height{};
	bool fromSTBI = false;
	unsigned char* pixels{};

	Texture(unsigned char* data, const unsigned int width, const unsigned int height);
	Texture(unsigned char* data, const unsigned int width, const unsigned int height, bool mssa);
	~Texture();

	glm::vec4 getSrc(float x, float y)
	{
		return glm::vec4(x / width, y / height, 128.0 / width, 128.0 / height);
	}

	//Texture interface
	bool SetData(unsigned char* data, const unsigned int width, const unsigned int height);
	bool Bind();
	bool Unbind();
};

#endif