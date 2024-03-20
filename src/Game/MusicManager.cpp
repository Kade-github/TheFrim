#include "MusicManager.h"
#include "Game.h"

MusicManager::MusicManager()
{
	instance = this;
}

void MusicManager::PlayMusic(std::string path)
{
	if (currentSong == path)
		return;

	currentSong = path;

	Channel& c = Game::instance->audioManager->CreateChannel(path, currentSong);

	c.Play();
}

