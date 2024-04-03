#include "TextureCache.h"
#include "stbi.h"

#include <vector>

std::map<std::string, Texture*> TextureCache::m_textures = {};

Texture* TextureCache::loadTexture(const std::string& path, bool flipped)
{
	Texture* t = NULL;

	if (flipped)
		t = stbi_h::stbi_load_file(path);
	else
		t = stbi_h::stbi_load_file_non_flipped(path);

	t->fromSTBI = true;

	if (stbi_h::get_error())
	{
		unsigned char c[] = { 0, 0, 0, 255 };
		delete t;
		Texture* tt = new Texture(c, 1, 1);
		tt->fromSTBI = false;
		return tt;
	}

	t->path = path;

	return t;
}

Texture* TextureCache::GetTexture(const std::string& path, bool flipped)
{
	if (m_textures.find(path) == m_textures.end())
	{
		Texture* t = loadTexture(path, flipped);
		AddTexture(path, t);
		return t;
	}
	else
	{
		return m_textures[path];
	}
	
}

void TextureCache::AddTexture(const std::string& path, Texture* texture)
{
	m_textures[path] = texture;
}

void TextureCache::RemoveTexture(const std::string& path)
{
	delete m_textures[path];
	m_textures.erase(path);
}

void TextureCache::Clear()
{
	std::vector<std::string> toRemove;
	for (auto& t : m_textures)
	{
		if (!t.second->dontDelete)
		{
			delete t.second;
			toRemove.push_back(t.first);
		}
	}

	for (auto& t : toRemove)
		m_textures.erase(t);
}
