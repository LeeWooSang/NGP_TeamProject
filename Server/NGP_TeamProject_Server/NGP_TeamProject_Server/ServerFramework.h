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
	// ��� ����
	SOCKET			m_listen_socket;

	// Ŭ������ ����
	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int addrlen = 0;

	// ���� ���¸� �����Ѵ�
	static byte gameState;

	static	vector<Client_Info> vec_client_info;

	static HANDLE hThread[2];

	static SkillManager* m_pSkillManager;
	static CServerFramework* p;


	static bool playerReady[2];
	// Ŭ���̾�Ʈ���� TPYE_START�� �ѹ��� �ް��ϱ� ���� �˻��ϴ� ����
	// playerCheck[0]�� PLAYER_1
	// playerCheck[1]�� PLAYER_2
	static bool playerCheck[2];

	static float FPS;
	static float elapsedTime;

	bool onSkill = false;
};

