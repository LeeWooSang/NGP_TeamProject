#include "ServerFramework.h"

CRITICAL_SECTION cs;

vector<Client_Info> CServerFramework::vec_client_info;
byte CServerFramework::gameState = TYPE_INIT;

HANDLE CServerFramework::hThread[2];

bool CServerFramework::gameover = false;

byte CServerFramework::winPlayer = 0;
float CServerFramework::FPS = 0.03;
float CServerFramework::elapsedTime = 0.f;
SkillManager* CServerFramework::m_pSkillManager{ nullptr };

CServerFramework::CServerFramework()
{
	InitializeCriticalSection(&cs);
	int retval = 0;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;
	cout << "윈속 초기화 성공!" << endl;

	// Socket( )
	// 대기 소켓 생성
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
	// namespace std에 있는 bind 함수와 Winsock2에 있는 bind 함수와 겹쳐서 구분
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
	cout << "대기 소켓 생성 완료" << endl;

	vec_client_info.reserve(2);
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

SOCKET& CServerFramework::AcceptClient()
{
	// accept( )
	addrlen = sizeof(client_addr);
	client_socket = accept(m_listen_socket, (SOCKADDR*)&client_addr, &addrlen);
	if (client_socket == INVALID_SOCKET)
		err_display("accept( )");

	cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
		<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;

	// 클라이언트 주소정보, 무슨 플레이어인지 벡터에 저장한다.
	if (vec_client_info.size() == 0)
	{
		EnterCriticalSection(&cs);
		COORD position;
		position.X = 0;
		position.Y = 550;
		vec_client_info.emplace_back(client_socket, PLAYER_1, position, IDLE, true,true,100);
		// 스킬의 좌표에 플레이어의 좌표를 넣어준다.
		vec_client_info[PLAYER_1].skillPos = vec_client_info[PLAYER_1].pos;
		hThread[PLAYER_1] = CreateThread(NULL, 0, RecvThread, (LPVOID)vec_client_info[PLAYER_1].client_socket, 0, NULL);
		LeaveCriticalSection(&cs);
	}
	else if (vec_client_info.size() == 1)
	{
		EnterCriticalSection(&cs);
		COORD position;
		position.X = 800;
		position.Y = 550;
		vec_client_info.emplace_back(client_socket, PLAYER_2, position, IDLE, true,true,100);
		// 스킬의 좌표에 플레이어의 좌표를 넣어준다.
		vec_client_info[PLAYER_2].skillPos = vec_client_info[PLAYER_2].pos;
		hThread[PLAYER_2] = CreateThread(NULL, 0, RecvThread, (LPVOID)vec_client_info[PLAYER_2].client_socket, 0, NULL);
		LeaveCriticalSection(&cs);
	}

	return client_socket;
}

DWORD WINAPI CServerFramework::RecvThread(LPVOID socket)
{
	SOCKET client_socket = (SOCKET)socket;
	int retval = 0;
	size_t packetSize = 0;

	while (true)
	{
		if (elapsedTime >= FPS)
		{
			switch (gameState)
			{
				case TYPE_RUN:
				{
					CS_RUN cs_runPacket;
					retval = recvn(client_socket, (char*)&cs_runPacket, sizeof(cs_runPacket), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display("recvn( )");
						return 0;
					}
					switch (cs_runPacket.player)
					{
						case PLAYER_1:
							KeyDistribute(cs_runPacket.player, cs_runPacket.key);
							break;
						case PLAYER_2:
							KeyDistribute(cs_runPacket.player, cs_runPacket.key);
							break;
					}
				}
				break;
			}
		}
		else
			elapsedTime += elapsedTime;
	}
return 0;
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
			EnterCriticalSection(&cs);
			vec_client_info[PLAYER_2].eMode = WALK;
			vec_client_info[PLAYER_1].isRight = true;
			vec_client_info[PLAYER_1].isSkillRight = true;
			LeaveCriticalSection(&cs);
		}
		else
		{
			EnterCriticalSection(&cs);
			vec_client_info[PLAYER_1].eMode = WALK;
			vec_client_info[PLAYER_2].isRight = true;
			vec_client_info[PLAYER_2].isSkillRight = true;
			LeaveCriticalSection(&cs);
		}

		EnterCriticalSection(&cs);
		vec_client_info[player].pos.X += 10;
		if (vec_client_info[player].pos.X > 1000)
		{
			vec_client_info[player].pos.X = 1000;
		}
		LeaveCriticalSection(&cs);
		break;
	case KEY_LEFT:
		if (player == PLAYER_1)
		{
			EnterCriticalSection(&cs);
			vec_client_info[PLAYER_2].eMode = WALK_B;
			vec_client_info[PLAYER_1].isRight = false;
			vec_client_info[PLAYER_1].isSkillRight = false;
			LeaveCriticalSection(&cs);
		}
		else
		{
			EnterCriticalSection(&cs);
			vec_client_info[PLAYER_1].eMode = WALK_B;
			vec_client_info[PLAYER_2].isRight = false;
			vec_client_info[PLAYER_2].isSkillRight = false;
			LeaveCriticalSection(&cs);
		}
		EnterCriticalSection(&cs);
		vec_client_info[player].pos.X -= 10;
		if (vec_client_info[player].pos.X < 0)
		{
			vec_client_info[player].pos.X = 0;
		}
		LeaveCriticalSection(&cs);
		break;
	case KEY_UP:
		if (player == PLAYER_1)
		{
			EnterCriticalSection(&cs);
			if (vec_client_info[PLAYER_1].isRight)
				vec_client_info[PLAYER_2].eMode = JUMP;
			else
				vec_client_info[PLAYER_2].eMode = JUMP_B;
			LeaveCriticalSection(&cs);
		}
		else
		{
			EnterCriticalSection(&cs);
			if (vec_client_info[PLAYER_2].isRight)
				vec_client_info[PLAYER_1].eMode = JUMP;
			else
				vec_client_info[PLAYER_1].eMode = JUMP_B;
			LeaveCriticalSection(&cs);
		}
		EnterCriticalSection(&cs);
		if (vec_client_info[player].pos.Y > 300)
			vec_client_info[player].pos.Y -= 150;
		LeaveCriticalSection(&cs);
		break;
	case KEY_SPACE:
		if (player == PLAYER_1)
		{
			EnterCriticalSection(&cs);
			if (vec_client_info[PLAYER_1].isRight)
				vec_client_info[PLAYER_2].eMode = ATTACK;
			else
				vec_client_info[PLAYER_2].eMode = ATTACK_B;
			LeaveCriticalSection(&cs);
		}
		else
		{
			EnterCriticalSection(&cs);
			if (vec_client_info[PLAYER_2].isRight)
				vec_client_info[PLAYER_1].eMode = ATTACK;
			else
				vec_client_info[PLAYER_1].eMode = ATTACK_B;
			LeaveCriticalSection(&cs);
		}
		EnterCriticalSection(&cs);
		m_pSkillManager->addSkill(player, vec_client_info[player].pos,vec_client_info[player].isSkillRight);
		LeaveCriticalSection(&cs);


		break;
	}
}

void CServerFramework::SendPacket(SOCKET& client_socket)
{
	int retval = 0;

	if (vec_client_info.size() == 2)
	{
		switch (gameState)
		{
		case TYPE_INIT:
		{
			SC_INIT sc_initPacket[2]{ 0 };
			for (int i = 0; i < vec_client_info.size(); ++i)
			{
				EnterCriticalSection(&cs);
				sc_initPacket[i].player = vec_client_info[i].player;
				sc_initPacket[i].isStart = true;
				LeaveCriticalSection(&cs);
				// 모든 플레이어에게 SC_INIT 고정길이 전송
				retval = send(vec_client_info[i].client_socket, (char*)&sc_initPacket[i], sizeof(SC_INIT), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send( )");
					return;
				}
			}
			gameState = TYPE_RUN;
		}
		break;

		case TYPE_RUN:
		{
			SC_RUN sc_runPacket = { 0 };

			EnterCriticalSection(&cs);
			sc_runPacket.eMode[PLAYER_1] = vec_client_info[PLAYER_1].eMode;
			sc_runPacket.eMode[PLAYER_2] = vec_client_info[PLAYER_2].eMode;
			sc_runPacket.pos[PLAYER_1] = vec_client_info[PLAYER_1].pos;
			sc_runPacket.pos[PLAYER_2] = vec_client_info[PLAYER_2].pos;

			for (int i = 0; i < MAXSKILL; i++) {
				sc_runPacket.skillInfo.player1_skill[i] = *m_pSkillManager->player1_Skill[i];
				sc_runPacket.skillInfo.player2_skill[i] = *m_pSkillManager->player2_Skill[i];
			}
			sc_runPacket.hp[PLAYER_1] = vec_client_info[PLAYER_1].hp;
			sc_runPacket.hp[PLAYER_2] = vec_client_info[PLAYER_2].hp;
			LeaveCriticalSection(&cs);

			
			for (int i = 0; i < vec_client_info.size(); ++i)
			{
				retval = send(vec_client_info[i].client_socket, (char*)&(sc_runPacket), sizeof(SC_RUN), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send( )");
					return;
				}

				EnterCriticalSection(&cs);
				if (vec_client_info[i].isRight)
				{
					if (i == PLAYER_1)
					{
						vec_client_info[PLAYER_2].eMode = IDLE;
					}
					else {
						vec_client_info[PLAYER_1].eMode = IDLE;
					}
				}
				else
				{
					if (i == PLAYER_1)
					{
						vec_client_info[PLAYER_2].eMode = IDLE_B;
					}
					else {
						vec_client_info[PLAYER_1].eMode = IDLE_B;
					}
				}
				LeaveCriticalSection(&cs);
			
			}

			if (sc_runPacket.hp[PLAYER_1]==0 || sc_runPacket.hp[PLAYER_2]==0)
			{

				if (sc_runPacket.hp[PLAYER_1] == 0)
				{
					winPlayer = 1;			//플레이어 2 우승
				}
				if (sc_runPacket.hp[PLAYER_2] == 0)
				{
					winPlayer = 0;			//플레이어 1 우승
				}
				
				//END부분
				gameState = TYPE_END;
				cout << "끝내기과정 돌입" << endl;
			}
		}
		break;
		case TYPE_END:
		{
			if (!gameover)
			{
				SC_END sc_endPacket = { 0 };

				sc_endPacket.winner = CServerFramework::winPlayer;

				for (int i = 0; i < vec_client_info.size(); i++) {
					retval = send(vec_client_info[i].client_socket, (char*)&sc_endPacket, sizeof(SC_END), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display("send( )");
						return;
					}
				}
				cout << "gameover\n";
				gameover = true;
			}
			break;
		}
		
		}
	}
}

void CServerFramework::Update()
{
	
	for (int i = 0; i < vec_client_info.size(); i++)
	{
		
		if (vec_client_info[i].pos.Y < 550.0f)
		{
			EnterCriticalSection(&cs);
			vec_client_info[i].pos.Y += 15;
			LeaveCriticalSection(&cs);
		}
		if (vec_client_info[i].pos.Y > 550.0f)
		{
			EnterCriticalSection(&cs);
			vec_client_info[i].pos.Y = 550.0f;
			LeaveCriticalSection(&cs);
		}
		
	}
	if (vec_client_info.size() > 1)
	{
		EnterCriticalSection(&cs);
		m_pSkillManager->update(vec_client_info[PLAYER_1], vec_client_info[PLAYER_2]);
		
		if (vec_client_info[PLAYER_1].hp ==0 || vec_client_info[PLAYER_2].hp==0)
		{
			for (int i = 0; i < MAXSKILL; i++) {
				m_pSkillManager->player1_Skill[i]->isEnable = false;
				m_pSkillManager->player1_Skill[i]->isCrush = true;
				m_pSkillManager->player1_Skill[i]->skillPos = { -100,-100 };


				m_pSkillManager->player2_Skill[i]->isEnable = false;
				m_pSkillManager->player2_Skill[i]->isCrush = true;
				m_pSkillManager->player2_Skill[i]->skillPos = { -100,-100 };

			}
		}
		LeaveCriticalSection(&cs);
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
