#ifndef _AUDIOMANAGER_H
#define _AUDIOMANAGER_H

#include <bass.h>
#include <bass_fx.h>

#include <string>
#include <vector>

class Channel {
	std::string _path;
	bool _autoFree = false;
public:
	static void EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
	{
		Channel* c = (Channel*)user;

		if (c->_autoFree)
			c->isFreed = true;

		c->isPlaying = false;
	}

	std::string name;
	unsigned long id = -1;

	bool isFreed = false;

	bool isPlaying = false;
	float volume = 1.0f;
	float length = 0.0f;

	Channel(std::string path, std::string _name, bool autoFree = false) {
		_path = path;
		name = _name;

		_autoFree = autoFree;
		
		auto flags = BASS_STREAM_PRESCAN | BASS_SAMPLE_FLOAT;

		if (autoFree)
			flags |= BASS_STREAM_AUTOFREE;

		id = BASS_StreamCreateFile(FALSE, _path.c_str(), 0, 0, flags);

		if (BASS_ErrorGetCode() != 0)
			id = -1;

		if (IsLoaded())
			length = BASS_ChannelBytes2Seconds(id, BASS_ChannelGetLength(id, BASS_POS_BYTE));

		BASS_ChannelSetSync(id, BASS_SYNC_END, 0, Channel::EndSync, this);
	}

	bool IsLoaded()
	{
		return id != -1;
	}

	void Play(bool restart = false)
	{
		if (!IsLoaded())
			return;

		BASS_ChannelPlay(id, restart);
		isPlaying = true;
	}

	void Stop()
	{
		if (!IsLoaded())
			return;

		BASS_ChannelStop(id);
		isPlaying = false;
	}

	void Pause()
	{
		if (!IsLoaded())
			return;

		BASS_ChannelPause(id);
		isPlaying = false;
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

		BASS_ChannelSetAttribute(id, BASS_ATTRIB_VOL, v);
		volume = vol;
	}

	void CreateFX()
	{
		if (!IsLoaded())
			return;

		id = BASS_FX_TempoCreate(id, BASS_FX_FREESOURCE);
	}

	void SetReverb(float mix, float time)
	{
		if (!IsLoaded())
			return;

		BASS_DX8_REVERB reverb;

		reverb.fInGain = -3.0f;
		reverb.fReverbMix = mix;
		reverb.fReverbTime = time;
		reverb.fHighFreqRTRatio = 0.001f;

		BASS_FXSetParameters(id, &reverb);
	}

	void Set3DReverb(float mix, float time)
	{
		if (!IsLoaded())
			return;

		BASS_DX8_I3DL2REVERB reverb;

		reverb.lRoom = -1000;
		reverb.lRoomHF = -100;
		reverb.flRoomRolloffFactor = 0.0f;
		reverb.flDecayTime = time;
		reverb.flDecayHFRatio = 0.1f;
		reverb.lReflections = -2602;
		reverb.flReflectionsDelay = 0.007f;
		reverb.lReverb = 200;
		reverb.flReverbDelay = 0.011f;
		reverb.flDiffusion = 100.0f;
		reverb.flDensity = 100.0f;
		reverb.flHFReference = 5000.0f;

		BASS_FXSetParameters(id, &reverb);
	}

	void Free()
	{
		if (IsLoaded())
		{
			BASS_ChannelStop(id);
			BASS_StreamFree(id);
		}

		id = -1;
	}

	bool operator==(const Channel& c)
	{
		return id == c.id;
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
		for (int i = 0; i < channels.size(); i++)
			channels[i].Free();

		channels.clear();

		BASS_Free();
	}

	Channel& CreateChannel(std::string path, std::string name, bool autoFree = false)
	{
		Channel c = Channel(path, name, autoFree);

		if (c.IsLoaded())
			channels.push_back(c);

		return channels[channels.size() - 1];
	}

	void RemoveChannel(Channel c)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i] == c)
			{
				if (channels[i].isFreed)
				{
					channels.erase(channels.begin() + i);
					break;
				}
				channels[i].Free();
				channels.erase(channels.begin() + i);
				break;
			}
		}
	}

	Channel& GetChannel(unsigned long id)
	{
		static Channel c("", "", false);

		c.isFreed = true;
		c.id = -1;

		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i].id == id)
			{
				if (channels[i].isFreed)
				{
					channels.erase(channels.begin() + i);
					break;
				}
				return channels[i];
			}
		}

		return c;
	}

	Channel& GetChannel(std::string name)
	{
		for (int i = 0; i < channels.size(); i++)
		{
			if (channels[i].name == name)
				return channels[i];
		}

		return channels[0];
	}
};

#endif