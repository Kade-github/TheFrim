#include <Game.h>

void Channel::CheckError()
{
	if (BASS_ErrorGetCode() != 0)
		Game::instance->log->Write("BASS Error: " + std::to_string(BASS_ErrorGetCode()) + " for " + name);
}

void Channel::Free()
{
	if (isFreed)
		return;

	RemoveFXHandles();
	BASS_ChannelStop(id);
	BASS_StreamFree(id);
	BASS_StreamFree(decode);

	id = -1;
}
