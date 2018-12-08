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
	// ��� ����
	SOCKET			m_listen_socket;
	// Ŭ������ ����
	SOCKET			client_socket;
	SOCKADDR_IN client_addr;
	int						addrlen = 0;

	// ���� ���¸� �����Ѵ�
	static byte								gameState;
	// Ŭ���̾�Ʈ ������ ������
	static vector<Client_Info>		vec_client_info;
	// Ŭ���̾�Ʈ ������ ��ü
	static HANDLE						m_clientThread[2];
	// ��ų���� �Ŵ���
	static SkillManager*				m_pSkillManager;
	// ���� �ð� ������
	static float								m_elapsedTime;
	// Ŭ���̾�Ʈ���� ��ũ�� ���߱� ���� ����
	static bool							sync[2];

	static byte								winPlayer;
};

