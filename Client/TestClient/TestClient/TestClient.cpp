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

	// ���� �ʱ�ȭ
	SC_INIT sc_initPacket;
	memset(&sc_initPacket, 0, sizeof(sc_initPacket));
	
	CS_RUN cs_runPacket;
	SC_RUN sc_runPacket;
	//cs_runPacket,sc_runPacket 0���� �ʱ�ȭ
	memset(&cs_runPacket, 0, sizeof(cs_runPacket));
	memset(&sc_runPacket, 0, sizeof(sc_runPacket));
	size_t packetSize = 0;

	byte gameState = TYPE_INIT;
	while (true)
	{
		switch (gameState)
		{
		// Init ����
		case TYPE_INIT:
			// �������� �ޱ�
			packetSize = sizeof(SC_INIT);
			retval = recvn(sock, (char*)&packetSize, sizeof(packetSize), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return 0;
			}
			// �������� �ޱ�
			retval = recvn(sock, (char*)&sc_initPacket, sizeof(sc_initPacket), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return 0;
			}

			if (sc_initPacket.type == TYPE_INIT)
			{
				if (sc_initPacket.isStart == true)
				{
					cout << "����� ���� ����!" << endl;
					gameState = TYPE_RUN;
					break;
				}
				else
				{
					cout << "��� ��ٸ��� ��!" << endl;
					//gameState = TYPE_INIT;
					break;
				}
			}
			else
				cout << "SC_INIT��Ŷ�� �ƴմϴ�." << endl;

			break;

		//Run ����
		case TYPE_RUN:
			while (true)
			{
				packetSize = sizeof(SC_RUN);
				// �������� �ʱ� �÷��̾� ��ġ �ޱ�
				retval = recvn(sock, (char*)&packetSize, sizeof(packetSize), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recv()");
					break;
				}
				// ���� ����
				retval = recvn(sock, (char*)&sc_runPacket, sizeof(sc_runPacket), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recv()");
					break;
				}
			
				cout << "�÷��̾� ��ġ ���� x: " << sc_runPacket.pos[PLAYER_1].X << ",";
				cout << "�÷��̾� ��ġ ���� y: " << sc_runPacket.pos[PLAYER_1].Y << endl;

				cout << "���� ��ġ ���� x: " << sc_runPacket.pos[PLAYER_2].X << ",";
				cout << "���� ��ġ ���� y: " << sc_runPacket.pos[PLAYER_2].Y << endl;

				//cs_runPacket�� player������ sc_initPacket���� �������� ���� �÷��̾� ��ȣ�� �״�� ����
				cs_runPacket.player = sc_initPacket.player;

				while (true)
				{
					Input_Keyboard(cs_runPacket);

					//Ű �Է��� ������쿡�� �����ϰ� �����ϱ����� 
					if (cs_runPacket.key != 0)
					{
						// �������� : ��Ŷ ũ�� ����
						packetSize = sizeof(cs_runPacket);
						retval = send(sock, (char*)&packetSize, sizeof(packetSize), 0);
						if (retval == SOCKET_ERROR)
						{
							err_display("send( )");
							return 0;
						}
						//cout << "��Ŷ ũ��(��������) ���� - " << retval << "Byte" << endl;

						// �������� : ���� ��Ŷ ����
						retval = send(sock, (char*)&cs_runPacket, sizeof(cs_runPacket), 0);
						if (retval == SOCKET_ERROR)
						{
							err_display("send( )");
							return 0;
						}
						Release_Key(cs_runPacket);
					}
				}
			}
			break;
		}

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

//void Print_Position()

void Input_Keyboard(CS_RUN& runPacket)
{
	runPacket.type = TYPE_RUN;
	//runPacket.player = PLAYER_1;

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
		//cout << myInfo.name << " - X : " << myInfo.pos.X << ", Y : " << myInfo.pos.Y << endl;
	}
	//else
	//{
	//	runPacket.key = KEY_IDLE;
	//	//cout << "Ű���� �� ����" << endl;
	//	break;
	//}
	
}

//Send �� Key�� ����
void Release_Key(CS_RUN& runPacket)
{
	if (runPacket.type == TYPE_RUN || runPacket.type == TYPE_SKILL)
	{
		if (runPacket.key == KEY_RIGHT || runPacket.key == KEY_UP || runPacket.key == KEY_SPACE || runPacket.key == KEY_LEFT)
		{
			runPacket.key = KEY_IDLE;
		}
	}
}