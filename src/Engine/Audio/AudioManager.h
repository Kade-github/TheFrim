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

	static void EndSync(HSYNC handle, DWORD channel, DWORD data, void* user);

	std::string name;
	unsigned long id = -1;
	unsigned long decode = -1;

	bool isFreed = false;

	float volume = 1.0f;
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

		id = BASS_StreamCreateFile(FALSE, _path.c_str(), 0, 0, BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT);

		CheckError();

		decode = BASS_StreamCreateFile(FALSE, _path.c_str(), 0, 0, BASS_STREAM_DECODE | BASS_SAMPLE_FLOAT);

		CheckError();

		if (IsLoaded())
			length = BASS_ChannelBytes2Seconds(id, BASS_ChannelGetLength(id, BASS_POS_BYTE));
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

		BASS_Set3DFactors(dist, -1.0f, -1.0f);
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

		if (vol >= 0.1f)
			vol = sqrt(vol);

		BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, v);

		CheckError();

		volume = vol;
	}

	void CreateFX()
	{
		if (!IsLoaded())
			return;

		id = BASS_FX_TempoCreate(decode, BASS_FX_FREESOURCE);

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
	std::vector<Channel> channels;

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

	Channel& CreateChannel(std::string path, std::string name, bool autoFree = false)
	{
        static Channel theC = Channel("NaA", "", false);
        theC.id = -1;

		Channel c = Channel(path, name, autoFree);

		if (c.IsLoaded())
			channels.push_back(c);

        if (channels.empty())
            return theC;

		return channels.back();
	}

	void RemoveChannel(Channel& c)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i].id == c.id)
			{
				if (channels[i].isFreed)
				{
					channels.erase(channels.begin() + i);
					break;
				}
				channels.erase(channels.begin() + i);
				break;
			}
		}
	}

	Channel& GetChannel(unsigned long id)
	{
		static Channel c("NaA", "", false);

		c.isFreed = true;
		c.id = -1;

		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i].id == id)
			{
				if (channels[i].autoFree && !channels[i].IsPlaying())
				{
					RemoveChannel(channels[i]);
					break;
				}
				return channels[i];
			}
		}

		return c;
	}

	Channel& GetChannel(std::string name)
	{
		static Channel c("NaA", "", false);

		c.isFreed = true;
		c.id = -1;

		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i].name == name)
			{
				if (channels[i].autoFree && !channels[i].IsPlaying())
				{
					RemoveChannel(channels[i]);
					break;
				}
				return channels[i];
			}
		}

		return c;
	}

	void RemoveStagnentChannels()
	{
		float time = glfwGetTime();
		for (int i = 0; i < channels.size(); i++)
		{
			Channel& c = channels[i];

			if (c.autoFree && !c.isFreed && !c.IsPlaying() && c.lifeTime + c.length < time)
			{
				RemoveChannel(c);
				break;
			}
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