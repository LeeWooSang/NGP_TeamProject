#include "Defines.h"
#include "ServerFramework.h"

int main()
{
	CServerFramework* Server = nullptr;
	Server = new CServerFramework;
	if (Server == nullptr)
		return 0;

	//DWORD WINAPI ServerThread()
	Server->AcceptClient();

	delete Server;
	Server = nullptr;
}