#include "ServerFramework.h"

CServerFramework* CServerFramework::p{ nullptr };
vector<Client_Info> CServerFramework::vec_client_info;
HANDLE CServerFramework::sendThread[2];
HANDLE CServerFramework::recieveThread[2];
HANDLE CServerFramework::sendEvent;
HANDLE CServerFramework::recieveEvent[2];
u_short CServerFramework::count = 0;


CRITICAL_SECTION cs;

DWORD CServerFramework::lastTime = timeGetTime();
float CServerFramework::FPS = 0.033;

CServerFramework::CServerFramework() : gameState(TYPE_INIT)
{
	InitializeCriticalSection(&cs);
	m_pSkillManager = new SkillManager();
}

CServerFramework::~CServerFramework()
{
	delete m_pSkillManager;
	DeleteCriticalSection(&cs);
	Destroy();
}

void CServerFramework::err_quit(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void CServerFramework::err_display(const char* msg)
{
	LPVOID lpMsgBuf;
	FormatMessage
	(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL
	);
	cout << msg << (char*)lpMsgBuf << endl;

	LocalFree(lpMsgBuf);
}

int CServerFramework::recvn(SOCKET s, char* buf, int len, int flags)
{
	int received;

	char *ptr = buf;
	int left = len;

	while (left > 0)
	{
		received = recv(s, ptr, left, flags);
		if (received == SOCKET_ERROR)
		{
			return SOCKET_ERROR;
		}
		else if (received == 0)
		{
			break;
		}
		left -= received;
		ptr += received;
	}

	return (len - left);
}


void CServerFramework::AcceptClient()
{
	InitializeCriticalSection(&cs);
	int retval = 0;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;
	cout << "���� �ʱ�ȭ ����!" << endl;

	// Socket( )
	// ��� ���� ����
	m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_socket == INVALID_SOCKET)
	{
		err_quit("socket( )");
		return;
	}

	// bind( )
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(serverPort);
	// namespace std�� �ִ� bind �Լ��� Winsock2�� �ִ� bind �Լ��� ���ļ� ����
	retval = ::bind(m_listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (retval == SOCKET_ERROR)
	{
		err_quit("bind( )");
		return;
	}

	// listen( )
	retval = listen(m_listen_socket, SOMAXCONN);
	if (retval == SOCKET_ERROR)
	{
		err_quit("listen( )");
		return;
	}

	cout << "��� ���� ���� �Ϸ�" << endl;

	// Ŭ������ ����
	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int addrlen;
	//HANDLE hThread;

	// Ŭ���̾�Ʈ ������ ������ ���� ����
	vec_client_info.reserve(2); 
	BOOL optval = true;

	
	// �̺�Ʈ ����
	// read�� write �Ѵ� �ڵ����� �̺�Ʈ�� ����
	// ������� �������� �ϳ��� �����常 �̺�Ʈ ������� �Ѱܹް� �ڵ����� ���ȣ ����
	sendEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	for (int i = 0; i < 2; i++) {
		
		recieveEvent[i] = CreateEvent(NULL, FALSE, FALSE, NULL);
	}
	while (true)
	{
		
		// accept( )
		addrlen = sizeof(client_addr);
		client_socket = accept(m_listen_socket, (SOCKADDR*)&client_addr, &addrlen);
		if (client_socket == INVALID_SOCKET)
		{
			err_display("accept( )");
			break;
		}
	
		cout << endl << "[ TCP ���� ] Ŭ���̾�Ʈ ���� - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", ��Ʈ ��ȣ : " << ntohs(client_addr.sin_port) << endl;

		size_t packetSize = sizeof(SC_INIT);

		// Ŭ���̾�Ʈ �ּ�����, ���� �÷��̾����� ���Ϳ� �����Ѵ�.
		if (vec_client_info.size() == 0)
		{
			COORD position;
			position.X = 0;
			position.Y = 550;
			//setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
			vec_client_info.emplace_back(client_socket, PLAYER_1, position, false, IDLE, false);
			// ��ų�� ��ǥ�� �÷��̾��� ��ǥ�� �־��ش�.
			vec_client_info[PLAYER_1].skillPos = vec_client_info[PLAYER_1].pos;


		}
		else if(vec_client_info.size() ==1 )
		{
			COORD position;			
			position.X = 800;
			position.Y = 550;
			//setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
			vec_client_info.emplace_back(client_socket, PLAYER_2, position, false, IDLE, false);
			// ��ų�� ��ǥ�� �÷��̾��� ��ǥ�� �־��ش�.
			vec_client_info[PLAYER_2].skillPos = vec_client_info[PLAYER_2].pos;
		}
		
		HANDLE hThread[4];
		if(vec_client_info.size() == 1)
			hThread[0]=sendThread[PLAYER_1] = CreateThread(NULL, 0, SendThread, (LPVOID)vec_client_info[PLAYER_1].client_socket, 0, NULL);
		else
		{
			
			hThread[1]=sendThread[PLAYER_2] = CreateThread(NULL, 0, SendThread, (LPVOID)vec_client_info[PLAYER_2].client_socket, 0, NULL);

			for (int i = 0,j=2; i < vec_client_info.size(),j<4; ++i,++j) 
			{
				hThread[j]=recieveThread[i] = CreateThread(NULL, 0, RecvThread,(LPVOID)vec_client_info[i].client_socket, 0, NULL);
				//CloseHandle(recieveThread[i]);
				WaitForMultipleObjects(4, hThread, TRUE, INFINITE);
			}
		}
	}
	
}

DWORD WINAPI CServerFramework::RecvThread(LPVOID socket)
{
	SOCKET client_socket = (SOCKET)socket;
	
	if(p == nullptr)
		p = new CServerFramework;
	
	while (true)
	{
		WaitForSingleObject(sendEvent, INFINITE);
		if (p->check == true)
		{
			
			if (p->gameState == TYPE_RUN || p->gameState == TYPE_START)
			{
				if ((SOCKET)socket == vec_client_info[PLAYER_1].client_socket)
					p->TestRecv(vec_client_info[PLAYER_1].client_socket);
				else if ((SOCKET)socket == vec_client_info[PLAYER_2].client_socket)
					p->TestRecv(vec_client_info[PLAYER_2].client_socket);
			}

		}
		SetEvent(recieveEvent);
	}

	return 0;
}


void CServerFramework::TestRecv(SOCKET& client_socket)
{
	int retval = 0;
	size_t packetSize = 0;

	switch (p->gameState)
	{
	case TYPE_START:
	{
		CS_INIT cs_initPacket;
		// ���Ͱ� ������ �ִ� �����̶� ���� recv�� client_socket���� � �������� ����
		// ����, PLAYER_1 ������ ��
		if (vec_client_info[PLAYER_1].client_socket == client_socket)
		{
			// ó������ PLAYER_1�� TYPE_START�� ���� ���̹Ƿ� if���� ���´�
			// ���� �� PLAYER_1�� TYPE_START�� ������ playerCheck�� true�̹Ƿ� recv ����
			if (playerCheck[PLAYER_1] == false)
			{
				retval = recvn(client_socket, (char*)&cs_initPacket, sizeof(cs_initPacket), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recvn( )");
					return;
				}

				if (cs_initPacket.player == PLAYER_1 && cs_initPacket.isReady == true)
					p->playerReady[PLAYER_1] = true;

				cout << "0���÷��̾�" << endl;
				// PLAYER_1�� TYPE_START ��Ŷ�� �޾����Ƿ� playerCheck[PLAYER_1]�� true�� �Ͽ�
				// ���̻� PLAYER_1�� TYPE_START�� ���ް� ��
				playerCheck[PLAYER_1] = true;
			}
		}
		// ���Ͱ� ������ �ִ� �����̶� ���� recv�� client_socket���� � �������� ����
		// ����, PLAYER_2 ������ ��
		else if(vec_client_info[PLAYER_2].client_socket == client_socket)
		{
			// ó������ PLAYER_2�� TYPE_START�� ���� ���̹Ƿ� if���� ���´�
			// ���� �� PLAYER_2�� TYPE_START�� ������ playerCheck�� true�̹Ƿ� recv ����
			if (playerCheck[PLAYER_2] == false)
			{
				retval = recvn(client_socket, (char*)&cs_initPacket, sizeof(cs_initPacket), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recvn( )");
					return;
				}

				if (cs_initPacket.player == PLAYER_2 && cs_initPacket.isReady == true)
					p->playerReady[PLAYER_2] = true;

				cout << "1���÷��̾�" << endl;
				// PLAYER_2�� TYPE_START ��Ŷ�� �޾����Ƿ� playerCheck[PLAYER_2]�� true�� �Ͽ�
				// ���̻� PLAYER_2�� TYPE_START�� ���ް� ��
				playerCheck[PLAYER_2] = true;
			}
		}

		if (p->playerReady[PLAYER_1] == true && p->playerReady[PLAYER_2] == true)
		{
			cout << "�Ѵ� �غ� ��" << endl;
			p->gameState = TYPE_RUN;
		}
		break;
	}
		
	case TYPE_RUN:
	
		CS_RUN cs_runPacket;
		retval = recvn(client_socket, (char*)&cs_runPacket, sizeof(cs_runPacket), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("recvn( )");
			return;
		}
		EnterCriticalSection(&cs);

		switch (cs_runPacket.player)
		{
		case PLAYER_1:
			KeyDistribute(cs_runPacket.player, cs_runPacket.key);
			//cout << "PLAYER_1 - X : " << vec_client_info[PLAYER_1].pos.X << ", Y : " << vec_client_info[PLAYER_1].pos.Y << endl;
			break;

		case PLAYER_2:
			KeyDistribute(cs_runPacket.player, cs_runPacket.key);
			//cout << "PLAYER_2 - X : " << vec_client_info[PLAYER_2].pos.X << ", Y : " << vec_client_info[PLAYER_2].pos.Y << endl;
			break;
		}
		LeaveCriticalSection(&cs);
		break;
	}
}
void CServerFramework::KeyDistribute(byte& player, byte& keyType)
{
	switch (keyType)
	{
	case KEY_IDLE:
		break;
	case KEY_RIGHT:
		if (player == PLAYER_1)
		{
			vec_client_info[PLAYER_2].eMode = WALK;
			vec_client_info[PLAYER_2].isBack = false;
		}
		else
		{
			vec_client_info[PLAYER_1].eMode = WALK;
			vec_client_info[PLAYER_1].isBack = false;
		}
		vec_client_info[player].pos.X += 10;
		break;
	case KEY_LEFT:
		if (player == PLAYER_1)
		{
			vec_client_info[PLAYER_2].eMode = WALK_B;
			vec_client_info[PLAYER_2].isBack = true;
		}
		else
		{
			vec_client_info[PLAYER_1].eMode = WALK_B;
			vec_client_info[PLAYER_1].isBack = true;
		}
		vec_client_info[player].pos.X -= 10;
		break;
	case KEY_UP:
		if (player == PLAYER_1)
		{
			if (vec_client_info[PLAYER_2].isBack)
				vec_client_info[PLAYER_2].eMode = JUMP_B;
			else
				vec_client_info[PLAYER_2].eMode = JUMP;
		}
		else
		{
			if (vec_client_info[PLAYER_1].isBack)
				vec_client_info[PLAYER_1].eMode = JUMP_B;
			else
				vec_client_info[PLAYER_1].eMode = JUMP;
		}
		if (vec_client_info[player].pos.Y > 300)
			vec_client_info[player].pos.Y -= 120;
		break;
	case KEY_SPACE:
		if (player == PLAYER_1)
		{
			if (vec_client_info[PLAYER_2].isBack)
				vec_client_info[PLAYER_2].eMode = ATTACK_B;
			else
				vec_client_info[PLAYER_2].eMode = ATTACK;
		}
		else
		{
			if (vec_client_info[PLAYER_1].isBack)
				vec_client_info[PLAYER_1].eMode = ATTACK_B;
			else
				vec_client_info[PLAYER_1].eMode = ATTACK;
		}
		for (int i = 0; i < 2; ++i)
			vec_client_info[i].onSkill = true;
		m_pSkillManager->addSkill(player, vec_client_info[player].pos);
		
		if(player == vec_client_info[PLAYER_1].player)
			cout << "�÷��̾�1 - ��ų �߻�" << endl;
		else
			cout << "�÷��̾�2 - ��ų �߻�" << endl;
		//vec_client_info[player].skillPos = vec_client_info[player].pos;
		//cout << "���̾ �߻�" << "," << player << endl;


		//cout << "Key - SPACE" << endl;
		break;
	}
}

DWORD WINAPI CServerFramework::SendThread(LPVOID socket)
{
	if(p == nullptr)
		p = new CServerFramework;

	if ((SOCKET)socket == vec_client_info[PLAYER_1].client_socket)
		p->SendPacket(vec_client_info[PLAYER_1].client_socket);
	double fElapsedTime = 0.0f;
	
	while (vec_client_info.size() > 1)
	{
		WaitForSingleObject(recieveEvent, INFINITE);
		float elapsedTime = (timeGetTime() - lastTime) * 0.001f;
		// 30������ 
		if (elapsedTime >= FPS)
		{
			
			p->Update(fElapsedTime);

			if ((SOCKET)socket == vec_client_info[PLAYER_2].client_socket)
				p->SendPacket(vec_client_info[PLAYER_2].client_socket);
			lastTime = timeGetTime();
			SetEvent(sendEvent);
		}
		//SetEvent(sendEvent);
	}

	return 0;
}

void CServerFramework::SendPacket(SOCKET& client_socket)
{
	int retval = 0;
	size_t packetSize = 0;
	switch (p->gameState)
	{
	case TYPE_INIT:
		packetSize = sizeof(SC_INIT);
		// Ŭ���̾�Ʈ �ּ�����, ���� �÷��̾����� ���Ϳ� �����Ѵ�.
		if (vec_client_info.size() == 1)
		{
			// SC_INIT ��Ŷ
			SC_INIT sc_initPacket;
			sc_initPacket.type = TYPE_INIT;
			sc_initPacket.player = vec_client_info[PLAYER_1].player;
			sc_initPacket.isStart = false;
			// PLAYER_1���� SC_INIT �������� ����
			retval = send(vec_client_info[PLAYER_1].client_socket, (char*)&sc_initPacket, sizeof(sc_initPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return;
			}
		}
		else
		{
			// SC_INIT ��Ŷ
			SC_INIT sc_initPacket[2];

			for (int i = 0; i < vec_client_info.size(); i++) 
			{
				sc_initPacket[i].type = TYPE_INIT;
				sc_initPacket[i].player = vec_client_info[i].player;
				sc_initPacket[i].isStart = true;

				// ��� �÷��̾�� SC_INIT �������� ����
				retval = send(vec_client_info[i].client_socket, (char*)&sc_initPacket[i], sizeof(sc_initPacket[i]), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send( )");
					return;
				}
			}
			p->gameState = TYPE_START;
			p->check = true;
		}
		break;

	case TYPE_RUN:
		SC_RUN sc_runPacket;
		sc_runPacket.type = TYPE_RUN;
		sc_runPacket.onSkill = false;				//�Ź� ���� ���� �켱 false�� �ʱ�ȭ
		if (vec_client_info.size() == 2)
		{
			for (int i = 0; i < vec_client_info.size(); ++i)
			{
				sc_runPacket.pos[i] = vec_client_info[i].pos;
				sc_runPacket.eMode[i] = vec_client_info[i].eMode;
				//std::cout << sc_runPacket.skillInfo.player1_skill[i].skillPos.X << "," << sc_runPacket.skillInfo.player1_skill[i].skillPos.Y << std::endl;
				//sc_runPacket.hp[i] = vec_client_info[i].hp;

				//sc_runPacket.onSkill = vec_client_info[i].onSkill;
				//sc_runPacket.pos[PLAYER_1] = vec_client_info[PLAYER_1].pos;
				//sc_runPacket.pos[PLAYER_2] = vec_client_info[PLAYER_2].pos;
				//�� Ŭ���̾�Ʈ�߿� ������ ��ų ī��Ʈ�� 0�̻��̸� �� Ŭ���̾�Ʈ���� onSkill = True�� �����Ѵ�.
				if (SkillManager::m_Player1_SkillCount > 0 || SkillManager::m_Player2_SkillCount > 0)
				{
					sc_runPacket.onSkill = true;

					if (SkillManager::m_Player1_SkillCount > 0)
					{
						//std::cout << sc_runPacket.skillInfo.player1_skill->skillPos.X <<","<< sc_runPacket.skillInfo.player1_skill->skillPos.Y << std::endl;

						for (int i = 0; i < MAXSKILL; i++)
						{
							sc_runPacket.skillInfo.player1_skill[i].skillPos = m_pSkillManager->player1->player1_skill[i].skillPos;
							//std::cout << sc_runPacket.skillInfo.player1_skill[i].skillPos.X << "," << sc_runPacket.skillInfo.player1_skill[i].skillPos.Y << std::endl;
						}
						
					}
					if (SkillManager::m_Player2_SkillCount > 0)
					{

						for (int i = 0; i < MAXSKILL; i++)
						{
							sc_runPacket.skillInfo.player2_skill[i].skillPos = m_pSkillManager->player2->player2_skill[i].skillPos;
							//std::cout << sc_runPacket.skillInfo.player2_skill[i].skillPos.X << "," << sc_runPacket.skillInfo.player2_skill[i].skillPos.Y << std::endl;
						}
						//std::cout << sc_runPacket.skillInfo.player2_skill->skillPos.X << "," << sc_runPacket.skillInfo.player2_skill->skillPos.Y << std::endl;
						//sc_runPacket.skillInfo.player2_skill->skillPos = m_pSkillManager->player2->player2_skill[i].skillPos;
						//std::cout << sc_runPacket.skillInfo.player2_skill->skillPos.X <<","<< sc_runPacket.skillInfo.player2_skill->skillPos.Y <<std::endl;
					}
					//std::cout <<"�÷��̾�1 ��ų ����"<< SkillManager::m_Player1_SkillCount << std::endl;
					//std::cout << "�÷��̾�2 ��ų ����" << SkillManager::m_Player2_SkillCount << std::endl;
				}
				else
				{
					sc_runPacket.onSkill = false;
				}
			}

			for (auto iter = vec_client_info.begin(); iter != vec_client_info.end(); ++iter)
			{
				// �÷��̾�1, 2���� SC_RUN �������� ����
				//cout << std::boolalpha << sc_runPacket.onSkill << "\n";
				retval = send((*iter).client_socket, (char*)&sc_runPacket, sizeof(sc_runPacket), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send( )");
					return;
				}
				//cout << "�÷��̾�1 : " << sc_runPacket.pos[PLAYER_1].X << ", " << sc_runPacket.pos[PLAYER_1].Y 
				//	<< "�÷��̾�2 : " << sc_runPacket.pos[PLAYER_2].X << ", " << sc_runPacket.pos[PLAYER_2].Y << endl;
				if (vec_client_info[PLAYER_1].isBack)
					vec_client_info[PLAYER_1].eMode = IDLE_B;
				else
					vec_client_info[PLAYER_1].eMode = IDLE;

				if (vec_client_info[PLAYER_2].isBack)
					vec_client_info[PLAYER_2].eMode = IDLE_B;
				else
					vec_client_info[PLAYER_2].eMode = IDLE;
			}
			//if (sc_runPacket.onSkill == true)
			//{
			//	/*packetSize = sizeof(SC_SKILL);
			//	for(int i=0;i<)*/				
			//}
		}
		break;
	}
}

void CServerFramework::Update(float elapsedTime)
{
	m_pSkillManager->update(elapsedTime,vec_client_info[PLAYER_1].pos,vec_client_info[PLAYER_2].pos);
	for (int i = 0; i < vec_client_info.size(); i++) 
	{
		if (vec_client_info[i].pos.Y < 550.0f)
		{
			vec_client_info[i].pos.Y += 15;
		}
		if (vec_client_info[i].pos.Y > 550.0f)
		{
			vec_client_info[i].pos.Y = 550.0f;
		}
	}

}

void CServerFramework::Destroy()
{
	for (auto iter = vec_client_info.begin(); iter != vec_client_info.end(); ++iter)
		closesocket(iter->client_socket);
	vec_client_info.clear();

	closesocket(m_listen_socket);
	WSACleanup();
}
