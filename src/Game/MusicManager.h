#ifndef _MUISC_MANAGER_H
#define _MUISC_MANAGER_H

#include <string>
#include <vector>
#include <glm/vec3.hpp>
#include <random>

class MusicManager
{
	float _fadeTime = 0.0f;
	float _fadeDuration = 0.0f;
	float _startVolume = 0.0f;
	bool _isPlaying = false;

	std::vector<std::string> trackList;

	std::default_random_engine generator;

public:
	bool pauseMusic = false;
	float nextTrack = 0.0f;

	float ourVolume = 1.0f;

	static MusicManager* GetInstance()
	{
		static MusicManager instance;
		return &instance;
	}

	bool ambient = false;
	bool dontFree = false;
	std::string currentSong = "";

	void FreeMusic();


	void ShuffleTracks();

	void GenerateTrackList();
	void GenerateAmbientTrackList();

	bool ChannelIsPlaying(std::string name);

	void PlayMusic(std::string path);
	void PlayMusic(std::string path, float fadeDuration);

	void SetReverb(float room, float damp, float wet);
	void SetCompression(float threshold, float ratio, float attack, float release);

	void RemoveFXs();

	void Set3DPosition(glm::vec3 pos, glm::vec3 front, glm::vec3 top);

	void PlaySFX(std::string path, std::string customName = "sfx");
	void PlaySFX(std::string path, glm::vec3 from, float pitch, std::string customName = "sfx");

	void FadeOut(float duration);

	void FadeTo(float volume, float duration);

	void PlayNext();

	bool IsPlaying();

	void Update();
};

#endif