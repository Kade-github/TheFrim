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

void MusicManager::ShuffleTracks()
{
	// shuffle

	for (int i = 0; i < trackList.size(); i++)
	{
		std::uniform_int_distribution<int> distribution(0, trackList.size() - 1);

		int j = distribution(generator);

		std::string temp = trackList[i];
		trackList[i] = trackList[j];
		trackList[j] = temp;
	}
}

void MusicManager::GenerateTrackList()
{
	generator.seed(time(nullptr));

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

	ShuffleTracks();
}

void MusicManager::GenerateAmbientTrackList()
{
	trackList.clear();

	trackList.push_back("ambientocclusion");
	trackList.push_back("imsotired");
	trackList.push_back("somethinglurks");

	ShuffleTracks();
}

bool MusicManager::ChannelIsPlaying(std::string name)
{
	Channel& c = Game::instance->audioManager->GetChannel(name);

	return c.IsPlaying();
}

void MusicManager::PlayMusic(std::string path)
{
	std::string rPath = "Assets/Music/tracks/" + path + ".mp3";
	if (currentSong == rPath)
		return;

	FreeMusic();

	currentSong = rPath;

	Channel& c = Game::instance->audioManager->CreateChannel(rPath, currentSong, true);

	c.SetReverb(0.5f, 0.5f);

	c.Play();

	_fadeTime = 0;
	_fadeDuration = 0;
}

void MusicManager::PlayMusic(std::string path, float fadeDuration)
{
	std::string rPath = "Assets/Music/tracks/" + path + ".mp3";

	if (ambient)
		rPath = "Assets/Music/amb/" + path + ".mp3";

	if (currentSong == rPath)
		return;

	FreeMusic();

	currentSong = rPath;

	Channel& c = Game::instance->audioManager->CreateChannel(rPath, currentSong, true);

	c.SetVolume(0);

	c.Play();

	_fadeTime = glfwGetTime();
	_fadeDuration = fadeDuration;

	// set next time by random

	int min = 120; // 2 minutes

	nextTrack = glfwGetTime() + c.length + min;

	std::uniform_int_distribution<int> distribution(min, 320); // 5 minute max

	int randomNumber = distribution(generator);

	nextTrack += randomNumber;

}

void MusicManager::PlaySFX(std::string path, std::string customName)
{
	std::string rPath = "Assets/Sfx/" + path + ".ogg";

	Channel& c = Game::instance->audioManager->CreateChannel(rPath, customName == "sfx" ? path : customName, true);

	c.Play();
}

void MusicManager::PlaySFX(std::string path, float pitch, std::string customName)
{
	std::string rPath = "Assets/Sfx/" + path + ".ogg";

	Channel& c = Game::instance->audioManager->CreateChannel(rPath, customName == "sfx" ? path : customName, true);

	c.CreateFX();

	c.SetPitch(pitch);

	c.Play();
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
	{
		if (ambient)
			GenerateAmbientTrackList();
		else
			GenerateTrackList();
	}

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
	static float lastCheck = 0;

	if (glfwGetTime() > nextTrack)
		PlayNext();

	if (Game::instance->audioManager->channels.size() == 0)
		return;

	if (glfwGetTime() > lastCheck)
	{
		lastCheck = glfwGetTime() + 0.15f;

		Game::instance->audioManager->RemoveStagnentChannels();
	}

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
}