#include "Defines.h"
#include "TestClient.h"

int main()
{
	int retval = 0;

	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 0;

	//socket
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET)
	{
		err_quit("socket( )");
		return 0;
	}
		
	//connect
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = ntohs(SERVERPORT);
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR)
	{
		err_quit("connect( )");
		return 0;
	}

	CS_RUN runPacket;
	while (true)
	{
		Input_Keyboard(runPacket);
		
		// �������� : ��Ŷ ũ�� ����
		size_t packetSize = sizeof(runPacket);
		retval = send(sock, (char*)&packetSize, sizeof(packetSize), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send( )");
			return 0;
		}
		//cout << "��Ŷ ũ��(��������) ���� - " << retval << "Byte" << endl;

		// �������� : ���� ��Ŷ ����
		retval = send(sock, (char*)&runPacket, sizeof(runPacket), 0);
		if (retval == SOCKET_ERROR)
		{
			err_display("send( )");
			return 0;
		}
		//cout << "���� ��Ŷ(��������) ���� - " << retval << "Byte" << endl;

	}
	
	closesocket(sock);
	WSACleanup();
}

void err_quit(const char *msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(const char *msg)
{
	LPVOID lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);

	printf("[%s] %s", msg, (char*)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

int recvn(SOCKET s, char* buf, int len, int flags)
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

void Input_Keyboard(CS_RUN& runPacket)
{
	runPacket.type = TYPE_RUN;
	runPacket.player = PLAYER_1;

	// kbhit( ) : ���ѷ��� �߿� Ű���尡 ���� ��� �Ҹ�
	if (kbhit())
	{
		// _getch( ) : ���;��� �ٷ� �Է��ϰ� ���ִ� �Լ�
		char key = _getch();
		switch (key)
		{
		case RIGHT:
			runPacket.key = KEY_RIGHT;
			cout << "RIGHT ����Ű ����" << endl;
			break;
		case LEFT:
			runPacket.key = KEY_LEFT;
			cout << "LEFT ����Ű ����" << endl;
			break;
		case UP:
			runPacket.key = KEY_UP;
			cout << "UP ����Ű ����" << endl;
			break;
		case SPACE:
			runPacket.key = KEY_SPACE;
			cout << "SPACE ����Ű ����" << endl;
			break;
		}
	}
	//else
	//{
	//	runPacket.key = KEY_IDLE;
	//	//cout << "Ű���� �� ����" << endl;
	//	break;
	//}
	
}