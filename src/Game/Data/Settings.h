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

	float fov = 90.0f;
	float renderDistance = 0.75f;

	float fogDistance = 0.75f;

	float masterVolume = 0.2f;

	bool vsync = true;
	bool fullscreen = false;

	bool useAmbientDiffuse = true;

	void Load();
	void Save();

	MSGPACK_DEFINE_ARRAY(fov, renderDistance, fogDistance, vsync, fullscreen, masterVolume, useAmbientDiffuse);
};

#endif