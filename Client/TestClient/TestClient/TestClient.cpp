#include "Defines.h"
#include "TestClient.h"

int main()
{
	int retval = 0;

	//윈속 초기화
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

	// 변수 초기화
	SC_INIT sc_initPacket;
	memset(&sc_initPacket, 0, sizeof(sc_initPacket));
	
	CS_RUN cs_runPacket;
	SC_RUN sc_runPacket;
	//cs_runPacket,sc_runPacket 0으로 초기화
	memset(&cs_runPacket, 0, sizeof(cs_runPacket));
	memset(&sc_runPacket, 0, sizeof(sc_runPacket));
	size_t packetSize = 0;

	byte gameState = TYPE_INIT;
	while (true)
	{
		switch (gameState)
		{
		// Init 상태
		case TYPE_INIT:
			// 고정길이 받기
			packetSize = sizeof(SC_INIT);
			retval = recvn(sock, (char*)&packetSize, sizeof(packetSize), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display("recvn( )");
				return 0;
			}
			// 가변길이 받기
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
					cout << "상대방과 연결 성공!" << endl;
					gameState = TYPE_RUN;
					break;
				}
				else
				{
					cout << "상대 기다리는 중!" << endl;
					//gameState = TYPE_INIT;
					break;
				}
			}
			else
				cout << "SC_INIT패킷이 아닙니다." << endl;

			break;

		//Run 상태
		case TYPE_RUN:
			while (true)
			{
				packetSize = sizeof(SC_RUN);
				// 서버에서 초기 플레이어 위치 받기
				retval = recvn(sock, (char*)&packetSize, sizeof(packetSize), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recv()");
					break;
				}
				// 가변 길이
				retval = recvn(sock, (char*)&sc_runPacket, sizeof(sc_runPacket), 0);
				if (retval == SOCKET_ERROR)
				{
					err_display("recv()");
					break;
				}
			
				cout << "플레이어 위치 정보 x: " << sc_runPacket.pos[PLAYER_1].X << ",";
				cout << "플레이어 위치 정보 y: " << sc_runPacket.pos[PLAYER_1].Y << endl;

				cout << "상대방 위치 정보 x: " << sc_runPacket.pos[PLAYER_2].X << ",";
				cout << "상대방 위치 정보 y: " << sc_runPacket.pos[PLAYER_2].Y << endl;

				//cs_runPacket에 player정보는 sc_initPacket에서 서버에서 받은 플레이어 번호를 그대로 대입
				cs_runPacket.player = sc_initPacket.player;

				while (true)
				{
					Input_Keyboard(cs_runPacket);

					//키 입력이 있을경우에만 전송하게 구현하기위해 
					if (cs_runPacket.key != 0)
					{
						// 고정길이 : 패킷 크기 전송
						packetSize = sizeof(cs_runPacket);
						retval = send(sock, (char*)&packetSize, sizeof(packetSize), 0);
						if (retval == SOCKET_ERROR)
						{
							err_display("send( )");
							return 0;
						}
						//cout << "패킷 크기(고정길이) 전송 - " << retval << "Byte" << endl;

						// 가변길이 : 실제 패킷 전송
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

	// kbhit( ) : 무한루프 중에 키보드가 눌린 경우 불림
	if (kbhit())
	{
		// _getch( ) : 엔터없이 바로 입력하게 해주는 함수
		char key = _getch();
		switch (key)
		{
		case RIGHT:
			runPacket.key = KEY_RIGHT;
			cout << "RIGHT 방향키 눌림" << endl;
			break;
		case LEFT:
			runPacket.key = KEY_LEFT;
			cout << "LEFT 방향키 눌림" << endl;
			break;
		case UP:
			runPacket.key = KEY_UP;
			cout << "UP 방향키 눌림" << endl;
			break;
		case SPACE:
			runPacket.key = KEY_SPACE;
			cout << "SPACE 방향키 눌림" << endl;
			break;
		}
		//cout << myInfo.name << " - X : " << myInfo.pos.X << ", Y : " << myInfo.pos.Y << endl;
	}
	//else
	//{
	//	runPacket.key = KEY_IDLE;
	//	//cout << "키보드 안 눌림" << endl;
	//	break;
	//}
	
}

//Send 한 Key값 해제
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