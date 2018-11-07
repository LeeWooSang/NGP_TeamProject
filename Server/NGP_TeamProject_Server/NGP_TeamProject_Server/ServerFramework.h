#pragma once
#include "Defines.h"

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

	vector<CS_RUN> vec_cs_runPacket;

	// ���� ���¸� �����Ѵ�
	byte gameState;
	// 0��° �ε����� PLAYER_1, 1��° �ε����� PLAYER_2
	static COORD playerPos[2];

	vector<Client_Info> vec_client_info;
};

