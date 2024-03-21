#include "MusicManager.h"
#include "Game.h"

#include <filesystem>


void MusicManager::FreeMusic()
{
	if (currentSong == "")
		return;

	Game::instance->audioManager->RemoveChannel(Game::instance->audioManager->GetChannel(currentSong));

	currentSong = "";

	_fadeTime = 0;
	_fadeDuration = 0;
	_isPlaying = false;
}

void MusicManager::GenerateTrackList()
{
	trackList.clear();

	std::string path = "Assets/Music/tracks/";

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		std::string p = entry.path().string();

		if (p.find(".mp3") != std::string::npos)
		{
			p = p.substr(path.size(), p.size() - path.size() - 4);
			trackList.push_back(p);
		}
	}

	// shuffle

	for (int i = 0; i < trackList.size(); i++)
	{
		int j = std::rand() % trackList.size();

		std::string temp = trackList[i];
		trackList[i] = trackList[j];
		trackList[j] = temp;
	}
}

void MusicManager::PlayMusic(std::string path)
{
	std::string rPath = "Assets/Music/tracks/" + path + ".mp3";
	if (currentSong == rPath)
		return;

	FreeMusic();

	currentSong = rPath;

	Channel& c = Game::instance->audioManager->CreateChannel(rPath, currentSong);

	c.SetReverb(0.5f, 0.5f);

	c.Play();

	_fadeTime = 0;
	_fadeDuration = 0;
}

void MusicManager::PlayMusic(std::string path, float fadeDuration)
{
	std::string rPath = "Assets/Music/tracks/" + path + ".mp3";
	if (currentSong == rPath)
		return;

	FreeMusic();

	currentSong = rPath;

	Channel& c = Game::instance->audioManager->CreateChannel(rPath, currentSong);

	c.SetVolume(0);

	c.SetReverb(0.5f, 0.5f);

	c.Play();

	_fadeTime = glfwGetTime();
	_fadeDuration = fadeDuration;

	// set next time by random

	int min = 240; // 4 minutes

	_nextTrack = glfwGetTime() + c.length + 240;

	_nextTrack += rand() % 400; // 6-10 minutes
}

void MusicManager::FadeOut(float duration)
{
	if (currentSong == "")
		return;

	Channel& c = Game::instance->audioManager->GetChannel(currentSong);

	_fadeTime = glfwGetTime();
	_fadeDuration = -duration;
	_startVolume = c.volume;

}

void MusicManager::PlayNext()
{
	if (trackList.size() == 0)
		GenerateTrackList();

	if (trackList.size() == 0)
		return;

	std::string path = trackList[0];
	trackList.erase(trackList.begin());

	PlayMusic(path, 6);
}

bool MusicManager::IsPlaying()
{
	return _isPlaying;
}

void MusicManager::Update()
{
	if (glfwGetTime() > _nextTrack)
		PlayNext();

	if (Game::instance->audioManager->channels.size() == 0)
		return;

	if (_fadeDuration > 0)
	{
		float t = (glfwGetTime() - _fadeTime) / _fadeDuration;

		if (t > 1)
		{
			_fadeDuration = 0;
			t = 1;
		}

		Channel& c = Game::instance->audioManager->GetChannel(currentSong);

		c.SetVolume(t);
	}

	if (_fadeDuration < 0)
	{
		float t = (glfwGetTime() - _fadeTime) / -_fadeDuration;

		if (t > 1)
		{
			_fadeDuration = 0;
			t = 1;

			FreeMusic();

			return;
		}

		Channel& c = Game::instance->audioManager->GetChannel(currentSong);

		c.SetVolume(_startVolume - t);
	}

	// if the song stopped

	if (!BASS_ChannelIsActive(Game::instance->audioManager->GetChannel(currentSong).id))
	{
		FreeMusic();
	}
}