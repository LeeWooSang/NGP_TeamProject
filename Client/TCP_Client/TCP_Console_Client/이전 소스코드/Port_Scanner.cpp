#pragma comment(lib, "ws2_32")
#include <iostream>
#include <string>
#include <algorithm>
#include <WinSock2.h>
using namespace std;

void err_quit(const char*);
void err_display(const char*);

int main(int argc, char* argv[])
{
	int data;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	string IP = argv[1];
	 
	int min = min(atoi(argv[2]), atoi(argv[3]));
	int max = max(atoi(argv[2]), atoi(argv[3]));
	
	if (min < 0)
		min = 0;
	if (max > 65535)
		max = 65535;
	
	u_short min_port = min;
	u_short max_port = max;

	do
	{
		SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
		if (client_socket == INVALID_SOCKET)
			err_quit("socket( )");

		SOCKADDR_IN server_addr;
		ZeroMemory(&server_addr, sizeof(server_addr));
		server_addr.sin_family = AF_INET;

		// string�� const char* ��ȯ
		server_addr.sin_addr.S_un.S_addr = inet_addr(IP.c_str());
		server_addr.sin_port = htons(min_port);

		cout << "�õ����� ��Ʈ��ȣ : " << min_port << endl;

		// connect( ) �Լ� ����
		data = connect(client_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
		// connect ���� ������ 0����
		
		if (data != SOCKET_ERROR)
			cout << "���� ���� ���� - ���� IP �ּ� : " << IP << ", ��Ʈ��ȣ : " << min_port << endl;
		else
			cout << "����� ��Ʈ ����" << endl;

		++min_port;

		closesocket(client_socket);

	} while (min_port <= max_port);

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
