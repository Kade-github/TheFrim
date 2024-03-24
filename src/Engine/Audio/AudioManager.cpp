#include <Game.h>

void Channel::EndSync(HSYNC handle, DWORD channel, DWORD data, void* user)
{
	Channel& c = Game::instance->audioManager->GetChannel(channel);

	if (c.autoFree)
	{
		Game::instance->audioManager->RemoveChannel(c);

		c.Free();
	}
}
