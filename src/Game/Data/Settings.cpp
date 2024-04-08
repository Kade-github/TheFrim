#include "Settings.h"
#include <fstream>

Settings* Settings::instance = new Settings();

void Settings::Load()
{
	// msgpack load from "settings.dat"

	std::ifstream file("settings.dat", std::ios::binary);

	if (file.is_open())
	{
		std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});

		msgpack::object_handle oh = msgpack::unpack(buffer.data(), buffer.size());

		msgpack::object obj = oh.get();

		obj.convert(*this);
	}
}

void Settings::Save()
{
	// msgpack save to "settings.dat"

	std::ofstream file("settings.dat", std::ios::binary);

	msgpack::sbuffer sbuf;

	msgpack::pack(sbuf, *this);

	file.write(sbuf.data(), sbuf.size());
}
