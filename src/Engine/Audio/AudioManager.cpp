#include <Game.h>

void Channel::CheckError()
{
	if (BASS_ErrorGetCode() != 0)
		Game::instance->log->Write("BASS Error: " + std::to_string(BASS_ErrorGetCode()) + " for " + name);
}