#include "Defines.h"
#include "ServerFramework.h"

int main()
{
	CServerFramework* Server = nullptr;
	Server = new CServerFramework;
	if (Server == nullptr)
		return 0;

	Server->ServerThread();

	delete Server;
	Server = nullptr;
}