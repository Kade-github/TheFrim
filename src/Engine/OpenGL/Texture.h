#pragma once

#ifndef _TEXTURE_H
#define _TEXTURE_H


#include "../../../Include/glad/glad.h"
#include <iostream>
#include <string>

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
	GLuint id{};

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

	static Texture* createWithImage(std::string filePath);

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

	//Texture interface
	bool SetData(unsigned char* data, const unsigned int width, const unsigned int height);
	bool Bind()
	{
		glBindTexture(GL_TEXTURE_2D, id);
		return false;
	}
	bool Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		return false;
	}
};

#endif