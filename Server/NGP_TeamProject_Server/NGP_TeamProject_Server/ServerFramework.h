#pragma once
#include "Defines.h"
#include "SkillManager.h"

//DWORD WINAPI RecvThread(LPVOID);

struct Client_Info
{
	Client_Info(SOCKET socket, byte num, COORD p, bool skill) : client_socket(socket), player(num), pos(p), onSkill(skill) {}

	SOCKET			client_socket;
	byte			player;
	COORD			pos;
	bool			onSkill;
	COORD			skillPos;
};

class CServerFramework
{
public:
	CServerFramework();
	~CServerFramework();

	static void err_quit(const char*);
	static void err_display(const char*);
	static int recvn(SOCKET, char*, int, int);

	void AcceptClient();
	
	static DWORD WINAPI RecvThread(LPVOID);
	void TestRecv(SOCKET&);
	void KeyDistribute(byte&, byte&);
	static DWORD WINAPI SendThread(LPVOID);
	
	void SendPacket(SOCKET&);
	void Update(float);

	void Destroy();
private:
	const char* serverIP = "127.0.0.1";
	const u_short	serverPort{ 9000 };
	// 대기 소켓
	SOCKET			m_listen_socket;

	SkillManager *m_pSkillManager;

	// 게임 상태를 저장한다
	byte gameState;
	// 0번째 인덱스는 PLAYER_1, 1번째 인덱스는 PLAYER_2
	static COORD playerPos[2];

	static	vector<Client_Info> vec_client_info;

	static HANDLE recieveThread[2];
	static HANDLE sendThread[2];
	static u_short count;

	static CServerFramework* p;
	bool check = false;

	bool playerReady[2] = { false, false };

	static DWORD lastTime;
	static float FPS;
};

