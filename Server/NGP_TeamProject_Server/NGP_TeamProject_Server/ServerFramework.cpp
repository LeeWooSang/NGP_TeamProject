#include "ServerFramework.h"

CServerFramework* CServerFramework::p{ nullptr };
CRITICAL_SECTION cs;

vector<Client_Info> CServerFramework::vec_client_info;
byte CServerFramework::gameState = TYPE_INIT;

HANDLE CServerFramework::hThread[2];

bool CServerFramework::playerCheck[2] = { false, false };
bool CServerFramework::playerReady[2] = { false, false };

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
	//m_pSkillManager = new SkillManager();
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
	//HANDLE hThread;
	// 클라이언트 정보를 저장한 공간 예약
	// 신호 상태
	//readEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	// 비신호 상태
	//writeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	// accept( )
	addrlen = sizeof(client_addr);
	client_socket = accept(m_listen_socket, (SOCKADDR*)&client_addr, &addrlen);
	if (client_socket == INVALID_SOCKET)
		err_display("accept( )");

	cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
		<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;

	size_t packetSize = sizeof(SC_INIT);

	// 클라이언트 주소정보, 무슨 플레이어인지 벡터에 저장한다.
	if (vec_client_info.size() == 0)
	{
		COORD position;
		position.X = 0;
		position.Y = 550;
		vec_client_info.emplace_back(client_socket, PLAYER_1, position, false);
		// 스킬의 좌표에 플레이어의 좌표를 넣어준다.
		vec_client_info[PLAYER_1].skillPos = vec_client_info[PLAYER_1].pos;
		hThread[PLAYER_1] = CreateThread(NULL, 0, RecvThread, (LPVOID)vec_client_info[PLAYER_1].client_socket, 0, NULL);
	}
	else if (vec_client_info.size() == 1)
	{
		COORD position;
		position.X = 800;
		position.Y = 550;
		vec_client_info.emplace_back(client_socket, PLAYER_2, position, false);
		// 스킬의 좌표에 플레이어의 좌표를 넣어준다.
		vec_client_info[PLAYER_2].skillPos = vec_client_info[PLAYER_2].pos;
		hThread[PLAYER_2] = CreateThread(NULL, 0, RecvThread, (LPVOID)vec_client_info[PLAYER_2].client_socket, 0, NULL);
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
			cout << elapsedTime << endl;
			switch (gameState)
			{
				case TYPE_START:
				{
					CS_INIT cs_initPacket;
					// 벡터가 가지고 있는 소켓이랑 현재 recv한 client_socket으로 어떤 소켓인지 구분
					// 만약, PLAYER_1 소켓일 때
					if (vec_client_info[PLAYER_1].client_socket == client_socket)
					{
						if (playerCheck[PLAYER_1] == false)
						{
							// 처음으로 PLAYER_1이 TYPE_START를 보낸 것이므로 if문에 들어온다
							// 만약 또 PLAYER_1이 TYPE_START를 보내면 playerCheck가 true이므로 recv 무시
							retval = recvn(client_socket, (char*)&cs_initPacket, sizeof(cs_initPacket), 0);
							if (retval == SOCKET_ERROR)
							{
								err_display("recvn( )");
								return 0;
							}
							cout << "0번플레이어" << endl;
							// PLAYER_1의 TYPE_START 패킷을 받았으므로 playerCheck[PLAYER_1]을 true로 하여
							// 더이상 PLAYER_1의 TYPE_START를 못받게 함

							if (cs_initPacket.isReady == true)
							{
								playerReady[PLAYER_1] = true;
								playerCheck[PLAYER_1] = true;
							}
						}
					}
					else if (vec_client_info[PLAYER_2].client_socket == client_socket)
					{
						if (playerCheck[PLAYER_2] == false)
						{
							retval = recvn(client_socket, (char*)&cs_initPacket, sizeof(cs_initPacket), 0);
							if (retval == SOCKET_ERROR)
							{
								err_display("recvn( )");
								return 0;
							}
							cout << "1번플레이어" << endl;
							if (cs_initPacket.isReady == true)
							{
								playerReady[PLAYER_2] = true;
								playerCheck[PLAYER_2] = true;
							}
						}
					}

					if (playerReady[PLAYER_1] == true && playerReady[PLAYER_2] == true)
					{
						cout << "둘다 준비 됨" << endl;
						gameState = TYPE_RUN;
					}
				}
				break;

				case TYPE_RUN:
				{
					static int num = 0;
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
							cout << "PLAYER_0 - X : " << vec_client_info[PLAYER_1].pos.X << ", Y : " << vec_client_info[PLAYER_1].pos.Y << endl;
							break;

						case PLAYER_2:
							KeyDistribute(cs_runPacket.player, cs_runPacket.key);
							cout << "PLAYER_1 - X : " << vec_client_info[PLAYER_2].pos.X << ", Y : " << vec_client_info[PLAYER_2].pos.Y << endl;
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

//DWORD WINAPI CServerFramework::SkillThread(LPVOID socket)
//{
//	SOCKET client_socket = (SOCKET)socket;
//
//	if (p == nullptr)
//	{
//		p = new CServerFramework;
//	}
//
//	int packetSize = sizeof(SC_SKILL);
//	int retval = 0;
//
//	if (SkillManager::m_Player1_SkillCount > 0)
//	{
//		for (int i = 0; i < MAXSKILL; i++)
//		{
//			retval = send()
//			p->m_pSkillManager->m_Player1_SkillCounT
//		}
//	}
//	
//	if (SkillManager::m_Player2_SkillCount > 0)
//	{
//		for (int i = 0; i < MAXSKILL; i++)
//		{
//
//		}
//
//	return 0;
//}

void CServerFramework::KeyDistribute(byte& player, byte& keyType)
{
	switch (keyType)
	{
	case KEY_IDLE:
		break;
	case KEY_RIGHT:
		vec_client_info[player].pos.X += 1;
		break;
	case KEY_LEFT:
		vec_client_info[player].pos.X -= 1;
		break;
	case KEY_UP:
		if (vec_client_info[player].pos.Y > 300)
			vec_client_info[player].pos.Y -= 30;
		break;
	case KEY_SPACE:
		for (int i = 0; i < 2; ++i)
			vec_client_info[i].onSkill = true;
		m_pSkillManager->addSkill(player, vec_client_info[player].pos);

		if (player == vec_client_info[PLAYER_1].player)
			cout << "플레이어1 - 스킬 발사" << endl;
		else
			cout << "플레이어2 - 스킬 발사" << endl;
		//vec_client_info[player].skillPos = vec_client_info[player].pos;
		//cout << "파이어볼 발사" << "," << player << endl;


		//cout << "Key - SPACE" << endl;
		break;
	}
}

void CServerFramework::SendPacket(SOCKET& client_socket)
{
	Update();
	int retval = 0;
	size_t packetSize = 0;

	if (vec_client_info.size() == 2)
	{
		switch (gameState)
		{
		case TYPE_INIT:
		{
			packetSize = sizeof(SC_INIT);
			SC_INIT sc_initPacket[2]{ 0 };
			for (int i = 0; i < vec_client_info.size(); ++i)
			{
				sc_initPacket[i].type = TYPE_INIT;
				sc_initPacket[i].player = vec_client_info[i].player;
				sc_initPacket[i].isStart = true;
				// 모든 플레이어에게 SC_INIT 고정길이 전송
				retval = send(vec_client_info[i].client_socket, (char*)&sc_initPacket[i], sizeof(SC_INIT), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send( )");
					return;
				}
			}
			cout << "둘다 TYPE_INIT 전송 됨" << endl;
			gameState = TYPE_START;
		}
		break;

		case TYPE_RUN:
		{
			SC_RUN sc_runPacket = { 0 };
			packetSize = sizeof(SC_RUN);
			sc_runPacket.type = TYPE_RUN;
			sc_runPacket.pos[PLAYER_1] = vec_client_info[PLAYER_1].pos;
			sc_runPacket.pos[PLAYER_2] = vec_client_info[PLAYER_2].pos;

			for (int i = 0; i < vec_client_info.size(); ++i)
				send(vec_client_info[i].client_socket, (char*)&(sc_runPacket), sizeof(SC_RUN), 0);
		}
		break;
		}
	}
}

void CServerFramework::Update()
{
	//m_pSkillManager->update(elapsedTime, vec_client_info[PLAYER_1].pos, vec_client_info[PLAYER_2].pos);
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
