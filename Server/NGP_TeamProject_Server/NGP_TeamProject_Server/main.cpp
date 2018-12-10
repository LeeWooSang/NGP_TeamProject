#include "Defines.h"
#include "ServerFramework.h"

DWORD lastTime = timeGetTime();
float FPS = 0.03;
int main()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	CServerFramework Server;

	SOCKET client_socket;
	float elapsedTime = 0.f;
	while (true)
	{
		elapsedTime = (timeGetTime() - lastTime) * 0.001f;
		Server.elapsedTime = elapsedTime;
		
		if (elapsedTime >= FPS)
		{
			if (Server.vec_client_info.size() < 2)
				client_socket = Server.AcceptClient();
			Server.Update();
			Server.SendPacket(client_socket);
			lastTime = timeGetTime();
		}
	}
}