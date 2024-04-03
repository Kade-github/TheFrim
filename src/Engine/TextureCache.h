#ifndef _TEXTURE_CACHE_H_
#define _TEXTURE_CACHE_H_

#include <map>
#include "OpenGL/Texture.h"

class TextureCache
{
	static std::map<std::string, Texture*> m_textures;

	static Texture* loadTexture(const std::string& path, bool flipped = false);

public:
	static Texture* GetTexture(const std::string& path, bool flipped = false);
	static void AddTexture(const std::string& path, Texture* texture);
	static void RemoveTexture(const std::string& path);
	static void Clear();
};

#endif