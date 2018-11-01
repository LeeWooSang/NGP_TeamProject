#pragma once
#include "Defines.h"

class CServerFramework
{
public:
	CServerFramework();
	~CServerFramework();

	static void err_quit(const char*);
	static void err_display(const char*);

	void AcceptClient();
	static DWORD WINAPI RecvThread(LPVOID);
	void Update(float);
	void SendPacket();
	void Destroy();

private:
	const u_short	ServerPort{ 9000 };
	// 대기 소켓
	SOCKET			m_listen_socket;

	int data;
};

