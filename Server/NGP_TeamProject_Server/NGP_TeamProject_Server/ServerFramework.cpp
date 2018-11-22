#include "ServerFramework.h"

CServerFramework* CServerFramework::p{ nullptr };
vector<Client_Info> CServerFramework::vec_client_info;
HANDLE CServerFramework::sendThread[2];
HANDLE CServerFramework::recieveThread[2];
u_short CServerFramework::count = 0;
HANDLE CServerFramework::readEvent;
HANDLE CServerFramework::writeEvent;

queue<CS_RUN> CServerFramework::que_client_key;					//Ű���� �Է��� ����.

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

	
	// ��ȣ ����
	//readEvent = CreateEvent(NULL, FALSE, TRUE, NULL);
	// ���ȣ ����
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
	
		cout << endl << "[ TCP ���� ] Ŭ���̾�Ʈ ���� - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", ��Ʈ ��ȣ : " << ntohs(client_addr.sin_port) << endl;

		size_t packetSize = sizeof(SC_INIT);

		// Ŭ���̾�Ʈ �ּ�����, ���� �÷��̾����� ���Ϳ� �����Ѵ�.
		if (vec_client_info.size() == 0)
		{
			COORD position;
			position.X = 0;
			position.Y = 0;
			//setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
			vec_client_info.emplace_back(client_socket, PLAYER_1, position, false);
			// ��ų�� ��ǥ�� �÷��̾��� ��ǥ�� �־��ش�.
			vec_client_info[PLAYER_1].skillPos = vec_client_info[PLAYER_1].pos;

			client_SockArray[PLAYER_1] = client_socket;

		}
		else if(vec_client_info.size() ==1 )
		{
			COORD position;			
			position.X = 800;
			position.Y = 0;
			//setsockopt(client_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&optval, sizeof(optval));
			vec_client_info.emplace_back(client_socket, PLAYER_2, position, false);
			// ��ų�� ��ǥ�� �÷��̾��� ��ǥ�� �־��ش�.
			vec_client_info[PLAYER_2].skillPos = vec_client_info[PLAYER_2].pos;

			client_SockArray[PLAYER_2] = client_socket;
		}
		
		
		if(vec_client_info.size() == 1)
			sendThread[PLAYER_1] = CreateThread(NULL, 0, SendThread, (LPVOID)vec_client_info[PLAYER_1].client_socket, 0, NULL);
		else
		{
			
			sendThread[PLAYER_2] = CreateThread(NULL, 0, SendThread, (LPVOID)vec_client_info[PLAYER_2].client_socket, 0, NULL);

			for (int i = 0; i < vec_client_info.size(); ++i) 
			{
				recieveThread[i] = CreateThread(NULL, 0, RecvThread, (LPVOID)vec_client_info[i].client_socket, 0, NULL);
				//CloseHandle(recieveThread[i]);
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
		if (p->check == true)
		{
			if (p->gameState == TYPE_RUN)
			{
				
				if ((SOCKET)socket == vec_client_info[PLAYER_1].client_socket)
					p->TestRecv(vec_client_info[PLAYER_1].client_socket);
				else if ((SOCKET)socket == vec_client_info[PLAYER_2].client_socket)
					p->TestRecv(vec_client_info[PLAYER_2].client_socket);
			}
		}
	}

	return 0;
}

void CServerFramework::TestRecv(SOCKET& client_socket)
{
	int retval = 0;
	size_t packetSize = 0;

	switch (p->gameState)
	{
	case TYPE_RUN:

		CS_RUN cs_runPacket;
		// �������� : ���� ��Ŷ �ޱ�
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
			cout << "PLAYER_1 - X : " << vec_client_info[PLAYER_1].pos.X << ", Y : " << vec_client_info[PLAYER_1].pos.Y << endl;
			break;

		case PLAYER_2:
			KeyDistribute(cs_runPacket.player, cs_runPacket.key);
			cout << "PLAYER_2 - X : " << vec_client_info[PLAYER_2].pos.X << ", Y : " << vec_client_info[PLAYER_2].pos.Y << endl;
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
		vec_client_info[player].pos.X += 10;
		break;
	case KEY_LEFT:
		vec_client_info[player].pos.X -= 10;
		break;
	case KEY_UP:
		vec_client_info[player].pos.Y += 200;			//
		break;
	case KEY_SPACE:
		vec_client_info[player].onSkill = true;
		cout << "Key - SPACE" << endl;
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
		clock_t begin = clock();
		p->Update(fElapsedTime);
		if ((SOCKET)socket == vec_client_info[PLAYER_2].client_socket)
			p->SendPacket(vec_client_info[PLAYER_2].client_socket);
		clock_t end = clock();

		fElapsedTime = double(end - begin) / CLOCKS_PER_SEC;
	}
//	timeEndPeriod(1);
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
		cout << "PLAYER_1 - X : " << sc_runPacket.pos[PLAYER_1].X << ", Y : " << sc_runPacket.pos[PLAYER_1].Y << endl;
	}

	else if (vec_client_info.size() >= 2) 
	{
		if (gameState == TYPE_RUN)
		{
			cout << "����" << endl;
			SC_RUN sc_runPacket;
			memset(&sc_runPacket, 0, sizeof(sc_runPacket));
			sc_runPacket.pos[0].X = 0;
			sc_runPacket.pos[0].Y = 0;

			sc_runPacket.pos[1].X = 800;
			sc_runPacket.pos[1].Y = 0;
			packetSize = sizeof(SC_RUN);
		


			for (auto iter = vec_client_info.begin(); iter != vec_client_info.end(); ++iter) {

				// �÷��̾�1, 2���� SC_RUN �������� ����
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
			p->gameState = TYPE_RUN;
		}
		break;

	case TYPE_RUN:
		SC_RUN sc_runPacket;

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


			// �÷��̾�1, 2���� SC_RUN �������� ����
			retval = send((*iter).client_socket, (char*)&sc_runPacket, sizeof(sc_runPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("sned( )");
				return;
			}
		}

		p->check = true;
		break;
	}
}

void CServerFramework::Update(float elapsedTime/*, byte& player*/)
{


	//vec_client_info[player].skillPos.X += 10 * elapsedTime;
	//cout << elapsedTime << endl;
	for (int i = 0; i < 2; i++) {
		if (vec_client_info[i].pos.Y > 0.0f)
		{
			vec_client_info[i].pos.Y -= elapsedTime;
		}
	}
	
	for (int i = 0; i < 2; i++) {
		if (vec_client_info[i].pos.Y < 0.0f)
		{
			vec_client_info[i].pos.Y = 0.0f;
		}
	}
	



}



void CServerFramework::Destroy()
{
	for (auto iter = vec_client_info.begin(); iter != vec_client_info.end(); ++iter)
		closesocket(iter->client_socket);

	closesocket(m_listen_socket);
	WSACleanup();
}