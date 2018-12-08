#include "Defines.h"
#include "ServerFramework.h"

DWORD	g_lastTime = timeGetTime();
float			g_FPS = 0.03;

int main()
{
	CServerFramework Server;
	float elapsedTime = 0.f;

	while (true)
	{
		elapsedTime = (timeGetTime() - g_lastTime) * 0.001f;
		Server.SetElapsedTime(elapsedTime);
		if (Server.GetClientInfo().size() < 2)
			Server.AcceptClient();

		if (elapsedTime >= g_FPS)
		{
			Server.Update();
			Server.SendPacket();
			g_lastTime = timeGetTime();
		}
	}
}