#pragma comment(lib, "ws2_32")
#include <iostream>
#include <WinSock2.h>
using namespace std;

void err_quit(const char*);
void err_display(const char*);
int recvn(SOCKET, char*, int, int);

const u_short ServerPort{ 9000 };
const int BufferSize{ 512 };
int main()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// Socket( )
	// 대기 소켓 생성
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
		err_quit("socket( )");

	cout << "대기 소켓 생성 완료" << endl;

	int data = 0;

	// bind( )
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(ServerPort);
	data = bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (data == SOCKET_ERROR)
		err_quit("bind( )");

	// listen( )
	data = listen(listen_socket, SOMAXCONN);
	if (data == SOCKET_ERROR)
		err_quit("listen( )");

	// 데이터 통신에 사용할 클라이언트 전용 소켓 생성
	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int addrlen;
	char buf[BufferSize + 1];
	int len;

	while (true)
	{
		// accept( )
		addrlen = sizeof(client_addr);
		client_socket = accept(listen_socket, (SOCKADDR*)&client_addr, &addrlen);
		if (client_socket == INVALID_SOCKET)
		{
			err_display("accept( )");
			break;
		}

		// 접속한 클라이언트 정보 출력
		cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;

		// 클라이언트와 데이터 통신
		while (true)
		{
			// 데이터 받기( recv( ) ) => 고정길이(받을 데이터 사이즈)
			data = recv(client_socket, (char*)&len, sizeof(int), 0);
			if (data == SOCKET_ERROR)
			{
				err_display("recvn( )");
				break;
			}
			else if (data == 0)
				break;

			// 실제 데이터 받기( recvn( ) ) => 가변 길이
			data = recvn(client_socket, buf, len, 0);
			if (data == SOCKET_ERROR)
			{
				err_display("recvn( )");
				break;
			}
			else if (data == 0)
				break;

			// 받은 데이터 출력
			buf[data] = '\0';
			cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port) << " ] - 받은 데이터 : " << buf << endl;
		}

		// closesocket( ) - 클라 전용 소켓 닫기
		closesocket(client_socket);
		cout << endl << "[ TCP 서버 ] 클라이언트 접속 - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", 포트 번호 : " << ntohs(client_addr.sin_port) << endl;
	}

	// closesocket( ) - 대기 소켓 닫기
	closesocket(listen_socket);

	// 윈속 종료
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

int recvn(SOCKET socket, char* buf, int len, int flag)
{
	int recev;
	char* data = buf;
	int left = len;

	while (left > 0)
	{
		recev = recv(socket, data, left, flag);
		if (recev == SOCKET_ERROR)
			return SOCKET_ERROR;
		else if (recev == 0)
			break;
		left -= recev;
		data += recev;
	}
	return (len - left);
}