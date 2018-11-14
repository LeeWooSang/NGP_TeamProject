#include "ServerFramework.h"

vector<Client_Info> CServerFramework::vec_client_info;
HANDLE CServerFramework::sendThread;
HANDLE CServerFramework::recieveThread[2];
u_short CServerFramework::count = 0;
HANDLE CServerFramework::readEvent;
HANDLE CServerFramework::writeEvent;

queue<CS_RUN> CServerFramework::que_client_key;					//키보드 입력이 들어간다.

CRITICAL_SECTION cs;

CServerFramework::CServerFramework() : gameState(TYPE_INIT)
{
	InitializeCriticalSection(&cs);
}

CServerFramework::~CServerFramework()
{
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

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
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

	// 클라전용 소켓
	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int addrlen;
	//HANDLE hThread;

	// 클라이언트 정보를 저장한 공간 예약
	vec_client_info.reserve(2); 
	BOOL optval = true;

	// 신호 상태
	//readEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	// 비신호 상태
	//writeEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
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
	
		cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;

		size_t packetSize = sizeof(SC_INIT);

		// 클라이언트 주소정보, 무슨 플레이어인지 벡터에 저장한다.
		if (vec_client_info.size() == 0)
		{
			COORD position;
			position.X = 0;
			position.Y = 0;
			setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
			vec_client_info.emplace_back(Client_Info(client_socket, PLAYER_1, position));
		
			client_SockArray[0] = client_socket;

		}
		else if(vec_client_info.size() ==1 )
		{
			COORD position;			
			position.X = 800;
			position.Y = 0;
			setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
			vec_client_info.emplace_back(Client_Info(client_socket, PLAYER_2, position));
			client_SockArray[1] = client_socket;
			//thread[vec_client_info.size() - 1] = CreateThread(NULL, 0, RecvThread, (LPVOID)&client_socket, 0, NULL);
		}
		
		for (int i = 0; i < vec_client_info.size(); ++i) {

			recieveThread[i] = CreateThread(NULL, 0, RecvThread, (LPVOID)vec_client_info[i].client_socket, 0, NULL);
			SetThreadPriority(recieveThread[i], THREAD_PRIORITY_TIME_CRITICAL);
			CloseHandle(recieveThread[i]);
		}

		
	}
}

DWORD WINAPI CServerFramework::RecvThread(LPVOID socket)
{
	

	SOCKET client_socket = (SOCKET)socket;
	CServerFramework* p = new CServerFramework;

	while (1) {
		if (p->gameState == TYPE_INIT)
		{
			
			p->SendPacket(client_socket);
			p->SendFirstPosition(client_socket);
			
		}
		else if (p->gameState == TYPE_RUN)
		{

			p->TestRecv(client_socket);
			p->SendPacket(client_socket);
		}
	}


	return 0;
}

void CServerFramework::SendFirstPosition(SOCKET& client_socket)
{

	int retval = 0;
	size_t packetSize = 0;
	SC_RUN sc_runPacket;
	CS_RUN cs_runPacket;
	sc_runPacket.type = TYPE_RUN;
	if (vec_client_info.size() == 1)
	{
		sc_runPacket.pos[PLAYER_1] = vec_client_info[PLAYER_1].pos;
		cout << sc_runPacket.pos[PLAYER_1].X << sc_runPacket.pos[PLAYER_1].Y << endl;
	}

	else if (vec_client_info.size() >= 2) {
		if (gameState == TYPE_RUN) {
			
			SC_RUN sc_runPacket;
			memset(&sc_runPacket, 0, sizeof(sc_runPacket));
			sc_runPacket.pos[0].X = 0;
			sc_runPacket.pos[0].Y = 0;

			sc_runPacket.pos[1].X = 800;
			sc_runPacket.pos[1].Y = 0;
			packetSize = sizeof(SC_RUN);
		
			// 플레이어1, 2에게 SC_RUN 고정길이 전송

			for (auto iter = vec_client_info.begin(); iter != vec_client_info.end(); ++iter) {
				//cout <<"클라에게 보낼 위치 좌표" <<iter->pos.X <<","<<iter->pos.Y<< endl;
				retval = send(iter->client_socket/*client_socket*/, (char*)&packetSize, sizeof(packetSize), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("send( )");
					return;
				}
				// 플레이어1, 2에게 SC_RUN 가변길이 전송
				retval = send(iter->client_socket/*client_socket*/, (char*)&sc_runPacket, sizeof(sc_runPacket), 0);
				if (retval == SOCKET_ERROR)
				{

					err_display("sned( )");
					return;
				}
			}
		}
	}

}

void CServerFramework::TestRecv(SOCKET& client_socket)
{
	int retval = 0;
	size_t packetSize = 0;

	if (gameState == TYPE_RUN)
	{
			// 고정길이 : 패킷크기 받기
		
			retval = recvn(client_socket, (char*)&packetSize, sizeof(packetSize), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return;
			}
			// CS_RUN 패킷
			CS_RUN cs_runPacket;
			// 가변길이 : 실제 패킷 받기
			retval = recvn(client_socket, (char*)&cs_runPacket, sizeof(cs_runPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return;
			}
			EnterCriticalSection(&cs);
			//que_client_key.push(cs_runPacket);
			switch (cs_runPacket.player)
			{
			case PLAYER_1:
				KeyDistribute(cs_runPacket.player, cs_runPacket.key);
				cout << "PLAYER_1 - X : " << vec_client_info[PLAYER_1].pos.X << ", Y : " << vec_client_info[PLAYER_1].pos.Y << endl;
				break;

			case PLAYER_2:
				KeyDistribute(cs_runPacket.player, cs_runPacket.key);
				cout << "PLAYER_2 - X : " << vec_client_info[PLAYER_2].pos.X << ", Y : " << vec_client_info[PLAYER_2].pos.Y << endl;
				break;
			}
			LeaveCriticalSection(&cs);
		
	}
	
}
void CServerFramework::SendPacket(SOCKET& client_socket)
{


	int retval = 0;
	size_t packetSize = 0;
	switch (gameState)
	{
	case TYPE_INIT:
		packetSize = sizeof(SC_INIT);
		// 클라이언트 주소정보, 무슨 플레이어인지 벡터에 저장한다.
		if (vec_client_info.size() == 1)
		{
			// SC_INIT 패킷
			SC_INIT sc_initPacket;
			sc_initPacket.type = TYPE_INIT;
			sc_initPacket.player = vec_client_info[PLAYER_1].player;
			sc_initPacket.isStart = false;
			//sc_initPacket.isStart = true;

			// PLAYER_1에게 SC_INIT 고정길이 전송
			retval = send(client_socket, (char*)&packetSize, sizeof(packetSize), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return;
			}
			// PLAYER_1에게 SC_INIT 가변길이 전송
			retval = send(client_socket, (char*)&sc_initPacket, sizeof(sc_initPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return;
			}
			//gameState = TYPE_RUN;
		}
		else
		{
			// SC_INIT 패킷
			SC_INIT sc_initPacket[2];

			for (int i = 0; i < vec_client_info.size(); i++) {
				sc_initPacket[i].type = TYPE_INIT;
				sc_initPacket[i].player = vec_client_info[i].player;
				sc_initPacket[i].isStart = true;


				// 모든 플레이어에게 SC_INIT 고정길이 전송
				retval = send(vec_client_info[i].client_socket, (char*)&packetSize, sizeof(packetSize), 0);

				if (retval == SOCKET_ERROR)
				{
					err_display("send( )");
					return;
				}
				// 모든 플레이어에게 SC_INIT 가변길이 전송
				retval = send(vec_client_info[i].client_socket, (char*)&sc_initPacket[i], sizeof(sc_initPacket[i]), 0);
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
		SC_RUN sc_runPacket;
		CS_RUN cs_runPacket;

		sc_runPacket.type = TYPE_RUN;
		if (vec_client_info.size() == 1)
		{
			sc_runPacket.pos[PLAYER_1] = vec_client_info[PLAYER_1].pos;
			cout << sc_runPacket.pos[PLAYER_1].X << sc_runPacket.pos[PLAYER_1].Y << endl;
		}

		else
		{
			sc_runPacket.pos[PLAYER_1] = vec_client_info[PLAYER_1].pos;
			sc_runPacket.pos[PLAYER_2] = vec_client_info[PLAYER_2].pos;
		}


		packetSize = sizeof(SC_RUN);
		for (auto iter = vec_client_info.begin(); iter != vec_client_info.end(); ++iter)
		{

			// 플레이어1, 2에게 SC_RUN 고정길이 전송
			retval = send(client_socket, (char*)&packetSize, sizeof(packetSize), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("send( )");
				return;
			}
			// 플레이어1, 2에게 SC_RUN 가변길이 전송
			retval = send(client_socket, (char*)&sc_runPacket, sizeof(sc_runPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("sned( )");
				return;
			}
		}
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
		vec_client_info[player].pos.X += 10;
		break;
	case KEY_LEFT:
		vec_client_info[player].pos.X -= 10;
		break;
	case KEY_UP:
		vec_client_info[player].pos.Y += 10;
		break;
	case KEY_SPACE:
		cout << "Key - SPACE" << endl;
		break;
	}
}

void CServerFramework::Update(float elapsedTime)
{
}



void CServerFramework::Destroy()
{
	for (auto iter = vec_client_info.begin(); iter != vec_client_info.end(); ++iter)
		closesocket(iter->client_socket);

	closesocket(m_listen_socket);
	WSACleanup();
}