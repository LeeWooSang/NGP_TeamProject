#pragma once
#include "Defines.h"
#include "SkillManager.h"

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
	static void KeyDistribute(byte&, byte&);
	static void Update();
	static void SendPacket();

	void Destroy();

	vector<Client_Info>& GetClientInfo()		const { return vec_client_info; }
	void SetElapsedTime(float elapsedTime) { m_elapsedTime = elapsedTime; }

private:
	const u_short	serverPort{ 9000 };
	// 대기 소켓
	SOCKET			m_listen_socket;
	// 클라전용 소켓
	SOCKET			client_socket;
	SOCKADDR_IN client_addr;
	int						addrlen = 0;

	// 게임 상태를 저장한다
	static byte								gameState;
	// 클라이언트 정보를 저장함
	static vector<Client_Info>		vec_client_info;
	// 클라이언트 스레드 객체
	static HANDLE						m_clientThread[2];
	// 스킬관리 매니저
	static SkillManager*				m_pSkillManager;
	// 서버 시간 관리용
	static float								m_elapsedTime;
	// 클라이언트끼리 싱크를 맞추기 위한 변수
	static bool							sync[2];

	static byte								winPlayer;
};

