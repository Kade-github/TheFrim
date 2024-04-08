#ifndef _SETTINGS_H
#define _SETTINGS_H

#include <string>
#include <vector>
#define MSGPACK_NO_BOOST
#include <msgpack.hpp>

class Settings
{
public:
	static Settings* instance;

	float fov = 75.0f;
	float renderDistance = 1.0f;

	float fogDistance = 1.0f;

	bool vsync = true;
	bool fullscreen = false;

	void Load();
	void Save();

	MSGPACK_DEFINE_ARRAY(fov, renderDistance, fogDistance, vsync, fullscreen);
};

#endif