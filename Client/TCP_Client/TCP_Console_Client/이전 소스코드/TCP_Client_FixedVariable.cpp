#pragma comment(lib, "ws2_32")
//
#pragma pack(1)

#include <iostream>
#include <string>
#include <WinSock2.h>
using namespace std;

void err_quit(const char*);
void err_display(const char*);
int recvn(SOCKET, char*, int, int);

const char* ServerIP{ "127.0.0.1" };
const u_short ServerPort{ 9000 };
const int BufferSize{ 50 };

int main()
{
	int data;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket == INVALID_SOCKET)
		err_quit("socket( )");

	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = inet_addr(ServerIP);
	server_addr.sin_port = htons(ServerPort);
	data = connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (data == SOCKET_ERROR)
		err_quit("connect( )");

	char buf[BufferSize];
	string testdata[4] =
	{
		"안녕하세요.",
		"반가워요.",
		"다시 만나요.",
		"Hello Network_Game_Programming!"
	};

	int len;

	// 서버와 통신
	for (int i = 0; i < 4; ++i)
	{
		// 데이터 입력
		len = testdata[i].size();
		strncpy(buf, testdata[i].c_str(), len);

		// 데이터 보내기( 데이터 크기 )
		data = send(client_socket, (char*)&len, sizeof(int), 0);
		if (data == SOCKET_ERROR)
		{
			err_display("send( )");
			break;
		}

		// 데이터 보내기(실제 데이터)
		// '\n' 문자 제거
		data = send(client_socket, buf, len, 0);
		if (data == SOCKET_ERROR)
		{
			err_display("send( )");
			break;
		}
		cout << "[ TCP 클라이언트 ] - " << data << "바이트를 보냈습니다." << endl;

	}

	closesocket(client_socket);
	WSACleanup();
}

void err_quit(const char* msg)
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

void err_display(const char* msg)
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

int recvn(SOCKET s, char* buf, int len, int flag)
{
	int recev;
	char* data = buf;
	int left = len;

	while (left > 0)
	{
		recev = recv(s, data, left, flag);
		if (recev == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (recev == 0)
			break;
		left -= recev;
		data += recev;
	}
	return (len - left);
}