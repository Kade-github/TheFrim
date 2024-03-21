#ifndef _MUISC_MANAGER_H
#define _MUISC_MANAGER_H

#include <string>
#include <vector>
class MusicManager
{
	float _fadeTime = 0.0f;
	float _fadeDuration = 0.0f;

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

	void PlayMusic(std::string path);
	void PlayMusic(std::string path, float fadeDuration);

	void FadeOut(float duration);

	void PlayNext();

	bool IsPlaying();

	void Update();
};

#endif