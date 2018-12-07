#pragma once
#include "Defines.h"
#include "SkillManager.h"

//DWORD WINAPI RecvThread(LPVOID);

struct Client_Info
{
	Client_Info(SOCKET socket, byte num, COORD p, bool skill) : client_socket(socket), player(num), pos(p), onSkill(skill) {}

	SOCKET	client_socket;
	byte			player;
	COORD	pos;
	bool			onSkill;
	COORD	skillPos;
};

class CServerFramework
{
public:
	CServerFramework();
	~CServerFramework();

	static void err_quit(const char*);
	static void err_display(const char*);
	static int recvn(SOCKET, char*, int, int);

	SOCKET& AcceptClient();
	static DWORD WINAPI RecvThread(LPVOID);
	static void KeyDistribute(byte&, byte&);

	//static DWORD WINAPI SkillThread(LPVOID);
	static void Update();
	static void SendPacket(SOCKET&);

	void Destroy();
//private:
public:
	const u_short	serverPort{ 9000 };
	// 대기 소켓
	SOCKET			m_listen_socket;

	// 클라전용 소켓
	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int addrlen = 0;

	// 게임 상태를 저장한다
	static byte gameState;

	static	vector<Client_Info> vec_client_info;

	static HANDLE hThread[2];

	static SkillManager* m_pSkillManager;
	static CServerFramework* p;


	static bool playerReady[2];
	// 클라이언트마다 TPYE_START를 한번만 받게하기 위해 검사하는 변수
	// playerCheck[0]은 PLAYER_1
	// playerCheck[1]은 PLAYER_2
	static bool playerCheck[2];

	static float FPS;
	static float elapsedTime;

	bool onSkill = false;
};

