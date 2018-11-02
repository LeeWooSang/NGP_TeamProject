#include "ServerFramework.h"

CServerFramework::CServerFramework()
{
}

CServerFramework::~CServerFramework()
{
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
	HANDLE hThread;

	// CS_RUN ��Ŷ
	CS_RUN runPacket;
	size_t packetSize = 0;
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

		while (true)
		{
			// �������� : ��Ŷũ�� �ޱ�
			retval = recvn(client_socket, (char*)&packetSize, sizeof(packetSize), 0);		
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return;
			}

			// �������� : ���� ��Ŷ �ޱ�
			
			retval = recvn(client_socket, (char*)&runPacket, sizeof(runPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return;
			}

			switch (runPacket.key)
			{
			case KEY_IDLE:
				cout << "Key - IDLE" << endl;
				break;
			case KEY_RIGHT:
				cout << "Key - RIGHT" << endl;
				break;
			case KEY_LEFT:
				cout << "Key - LEFT" << endl;
				break;
			case KEY_UP:
				cout << "Key - UP" << endl;
				break;
			case KEY_SPACE:
				cout << "Key - SPACE" << endl;
				break;
			}

			runPacket.key = KEY_IDLE;  // KEY_IDLE == 0x00�� 0�� ���� �ؼ� �ϴ� ���� 
		}
		
		closesocket(client_socket);
		//hThread = CreateThread(NULL, 0, RecvThread, (LPVOID)client_socket, 0, NULL);
		//if (hThread == nullptr)
		//	break;
		//else
		//	CloseHandle(hThread);
	}
	closesocket(m_listen_socket);

	WSACleanup();

	//return 0;
}

DWORD WINAPI CServerFramework::RecvThread(LPVOID arg)
{
	return 0;
}

void CServerFramework::Update(float elapsedTime)
{

}

void CServerFramework::SendPacket()
{

}

void CServerFramework::Destroy()
{
	closesocket(m_listen_socket);
	WSACleanup();
}