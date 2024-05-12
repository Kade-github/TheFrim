#ifndef _AUDIOMANAGER_H
#define _AUDIOMANAGER_H

#include <bass.h>
#include <bass_fx.h>
#include <glm/vec3.hpp>
#include <iostream>

#include <string>
#include <vector>
#include <glfw/glfw3.h>

class Channel {
	std::string _path;

	void CheckError();
public:
	bool autoFree = false;

	float lifeTime = 0;

	std::vector<unsigned int> fxHandles = {};

	std::string name;
	unsigned long id = -1;
	unsigned long decode = -1;

	bool isFreed = false;

	float mixVolume = 1.0f;
	float volume = 1.0f;
	float realVolume = 1.0f;
	float length = 0.0f;
	float pitch = 1.0f;

	~Channel()
	{
		Free();
	}

	Channel(std::string path, std::string _name, bool autoFree = false) {

		if (path == "NaA")
		{
			return;
		}

		lifeTime = glfwGetTime();

		_path = path;
		name = _name;

		auto flags = BASS_STREAM_PRESCAN;

		if (IsStereo())
			flags |= BASS_SAMPLE_FLOAT;
		else
			flags |= BASS_SAMPLE_MONO | BASS_SAMPLE_3D;

		id = BASS_StreamCreateFile(FALSE, _path.c_str(), 0, 0, flags);

		if (!IsStereo())
		{
			BASS_ChannelSet3DAttributes(id, BASS_3DMODE_OFF, -1, -1, -1, -1, 1);
		}

		CheckError();

		decode = BASS_StreamCreateFile(FALSE, _path.c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

		CheckError();

		if (IsLoaded())
		{
			length = BASS_ChannelBytes2Seconds(id, BASS_ChannelGetLength(id, BASS_POS_BYTE));
		}
	}

	bool IsLoaded()
	{
		return id != -1;
	}

	bool IsPlaying()
	{
		if (!IsLoaded())
			return false;

		return BASS_ChannelIsActive(id) == BASS_ACTIVE_PLAYING;
	
	}

	void Play(bool restart = false)
	{
		if (!IsLoaded())
			return;

		BASS_ChannelPlay(id, restart);
		CheckError();
	}

	void Stop()
	{
		if (!IsLoaded())
			return;

		BASS_ChannelStop(id);
		CheckError();
	}

	void Pause()
	{
		if (!IsLoaded())
			return;

		BASS_ChannelPause(id);
		CheckError();
	}

	void Set3DDistanceFactor(float dist)
	{
		if (!IsLoaded())
			return;

		BASS_Set3DFactors(-1, pow(2, (dist - 10) / 5.0), -1);

		BASS_ChannelSet3DAttributes(id, BASS_3DMODE_NORMAL, 0, 8, -1, -1, 0);
		CheckError();
	}

	void SetVolume(float vol)
	{
		if (!IsLoaded())
			return;
		float v = vol;

		if (vol <= 0)
			v = 0;

		if (vol >= 1)
			v = 1;

		realVolume = fmin(mixVolume, v);

		BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, realVolume);

		CheckError();

		volume = vol;
	}

	bool IsStereo()
	{
		if (!IsLoaded())
			return false;

		BASS_CHANNELINFO info;

		BASS_ChannelGetInfo(id, &info);

		return info.chans == 2 || _path.find(".mp3") != std::string::npos;
	}

	void CreateFX()
	{
		if (!IsLoaded())
			return;

		auto flags = BASS_FX_FREESOURCE | BASS_STREAM_PRESCAN;

		if (IsStereo())
			flags |= BASS_SAMPLE_FLOAT;
		else
			flags |= BASS_SAMPLE_MONO | BASS_SAMPLE_3D;

		id = BASS_FX_TempoCreate(decode, flags);

		CheckError();
	}

	void RemoveFXHandles()
	{
		for (int i = 0; i < fxHandles.size(); i++)
			BASS_ChannelRemoveFX(id, fxHandles[i]);

		fxHandles.clear();
	}

	void RemoveFXHandle(int handle)
	{
		BASS_ChannelRemoveFX(id, handle);

		for (int i = 0; i < fxHandles.size(); i++)
		{
			if (fxHandles[i] == handle)
			{
				fxHandles.erase(fxHandles.begin() + i);
				break;
			}
		}
	}


	int SetReverb(float room, float damp, float wet)
	{
		if (!IsLoaded())
			return -1;

		BASS_DX8_REVERB reverb;

		reverb.fInGain = 0.0f;
		reverb.fReverbMix = wet;
		reverb.fReverbTime = room;

		reverb.fHighFreqRTRatio = damp;


		int fxHandle = BASS_ChannelSetFX(id, BASS_FX_DX8_REVERB, 1);

		BASS_FXSetParameters(fxHandle, &reverb);

		CheckError();

		fxHandles.push_back(fxHandle);

		return fxHandle;
	}

	int SetCompression(float threshold, float ratio, float attack, float release, float gain)
	{
		if (!IsLoaded())
			return -1;

		BASS_DX8_COMPRESSOR compressor;

		compressor.fGain = gain;
		compressor.fAttack = attack;
		compressor.fRelease = release;
		compressor.fThreshold = threshold;
		compressor.fRatio = ratio;

		int fxHandle = BASS_ChannelSetFX(id, BASS_FX_DX8_COMPRESSOR, 1);

		BASS_FXSetParameters(fxHandle, &compressor);

		CheckError();

		fxHandles.push_back(fxHandle);

		return fxHandle;
	}

	void SetPitch(float p)
	{
		if (!IsLoaded())
			return;

		float sample;
		BASS_ChannelGetAttribute(id, BASS_ATTRIB_FREQ, &sample);

		BASS_ChannelSetAttribute(id, BASS_ATTRIB_FREQ, sample * p);

		CheckError();

		pitch = p;
	}


	void Free()
	{
		if (isFreed)
			return;

		if (IsLoaded())
		{
			RemoveFXHandles();
			BASS_ChannelStop(id);
			BASS_StreamFree(id);


		}

		id = -1;
	}
};

class AudioManager {
public:
	std::vector<Channel*> channels;

	float volume = 1.0f;

	AudioManager()
	{
		BASS_Init(-1, 44100, 0, 0, NULL);
		BASS_SetConfig(BASS_CONFIG_ASYNCFILE_BUFFER, 10000);
	}

	~AudioManager()
	{
		channels.clear();

		BASS_Free();
	}

	Channel* CreateChannel(std::string path, std::string name, bool autoFree = false)
	{
  
		Channel* c = new Channel(path, name, autoFree);

		channels.push_back(c);

		if (channels.empty())
			return nullptr;

		c->mixVolume = fmin(BASS_GetVolume(), volume);
		c->SetVolume(c->volume);

		return c;
	}

	void RemoveChannel(Channel* c)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i]->id == c->id)
			{
				if (channels[i]->isFreed)
				{
					channels.erase(channels.begin() + i);
					break;
				}
				channels.erase(channels.begin() + i);
				delete c;
				break;
			}
		}
	}

	Channel* GetChannel(unsigned long id)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i]->id == id)
			{
				if (channels[i]->autoFree && !channels[i]->IsPlaying())
				{
					RemoveChannel(channels[i]);
					break;
				}
				return channels[i];
			}
		}

		return nullptr;
	}

	Channel* GetChannel(std::string name)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i]->name == name)
			{
				if (channels[i]->autoFree && !channels[i]->IsPlaying())
				{
					RemoveChannel(channels[i]);
					break;
				}
				return channels[i];
			}
		}

		return nullptr;
	}

	void RemoveStagnentChannels()
	{
		float time = glfwGetTime();
		for (int i = 0; i < channels.size(); i++)
		{
			Channel* c = channels[i];

			if (!c->isFreed && !c->IsPlaying() && c->lifeTime + c->length < time)
			{
				RemoveChannel(c);
				break;
			}
		}
	}

	void SetVolume(float vol)
	{
		if (vol <= 0)
			vol = 0;

		if (vol >= 1)
			vol = 1;

		volume = vol;

		for (int i = 0; i < channels.size(); i++)
		{
			Channel* c = channels[i];

			c->mixVolume = fmin(BASS_GetVolume(), vol);

			c->SetVolume(c->volume);
		}
	}

	void Set3DPosition(glm::vec3 _pos, glm::vec3 _front, glm::vec3 _top)
	{
		BASS_3DVECTOR pos = { _pos.x, _pos.y, _pos.z };
		BASS_3DVECTOR front = { _front.x, _front.y, _front.z };
		BASS_3DVECTOR top = { _top.x, _top.y, _top.z };

		BASS_Set3DPosition(&pos, NULL, &front, &top);
	}

	void Apply3D()
	{
		BASS_Apply3D();
	}
};

#endif