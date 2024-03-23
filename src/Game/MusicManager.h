#ifndef _MUISC_MANAGER_H
#define _MUISC_MANAGER_H

#include <string>
#include <vector>
class MusicManager
{
	float _fadeTime = 0.0f;
	float _fadeDuration = 0.0f;
	float _startVolume = 0.0f;
	float _nextTrack = 0.0f;

	bool _isPlaying = false;

	std::vector<std::string> trackList;

public:
	static MusicManager* GetInstance()
	{
		static MusicManager instance;
		return &instance;
	}

	std::string currentSong = "";

	void FreeMusic();

	void GenerateTrackList();

	bool ChannelIsPlaying(std::string name);

	void PlayMusic(std::string path);
	void PlayMusic(std::string path, float fadeDuration);

	void PlaySFX(std::string path, std::string customName = "sfx");

	void FadeOut(float duration);

	void PlayNext();

	bool IsPlaying();

	void Update();
};

#endif