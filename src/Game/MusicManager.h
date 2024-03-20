#ifndef _MUISC_MANAGER_H
#define _MUISC_MANAGER_H

#include <string>

class MusicManager
{
public:
	static MusicManager* instance;

	std::string currentSong = "";

	MusicManager();

	void PlayMusic(std::string path);
};

#endif