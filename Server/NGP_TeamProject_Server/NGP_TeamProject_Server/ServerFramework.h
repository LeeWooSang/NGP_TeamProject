#pragma once
#include "Defines.h"


DWORD WINAPI RecvThread(LPVOID);


struct Client_Info
{
	Client_Info(SOCKET socket, byte num, COORD p) : client_socket(socket), player(num), pos(p)  {}

	SOCKET client_socket;
	byte player;
	COORD pos;
};

class CServerFramework
{
public:
	CServerFramework();
	~CServerFramework();

	static void err_quit(const char*);
	static void err_display(const char*);
	int recvn(SOCKET, char*, int, int);

	void AcceptClient();
	
	static DWORD WINAPI RecvThread(LPVOID);
	static DWORD WINAPI SendThread(LPVOID);

	void TestRecv(SOCKET&);
	void KeyDistribute(byte&, byte&);

	void Update(float);
	void SendPacket();
	void Destroy();

	
	
private:
	const char* serverIP = "127.0.0.1";
	const u_short	serverPort{ 9000 };
	// ��� ����
	SOCKET			m_listen_socket;

	// ���� ���¸� �����Ѵ�
	byte gameState;
	// 0��° �ε����� PLAYER_1, 1��° �ε����� PLAYER_2
	static COORD playerPos[2];

	static vector<Client_Info> vec_client_info;

	static HANDLE sendThread;
	static HANDLE recieveThread[2];
	static u_short count;

	static HANDLE readEvent;
	static HANDLE writeEvent;

	static const int clientNum = 2;
	SOCKET client_SockArray[clientNum];
	

};

