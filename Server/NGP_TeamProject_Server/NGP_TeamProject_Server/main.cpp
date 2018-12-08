#include "Defines.h"
#include "ServerFramework.h"

DWORD lastTime = timeGetTime();
float FPS = 0.03;
int main()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

	// 비신호 상태
	//hEvent = CreateEvent(NULL, true, TRUE, NULL);	// recv가 끝나기를 기다리는 이벤트

	CServerFramework Server;

	SOCKET client_socket;
	float elapsedTime = 0.f;
	int retval;
	while (true)
	{
		//retval = WaitForSingleObject(hEvent, INFINITE);
		//if (retval == WAIT_OBJECT_0)
		//{
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
			
		//}
		//else
		//	cout << "주 스레드 wait*() 오류 발생\n";
	}
}