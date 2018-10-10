#pragma comment(lib, "ws2_32")
#pragma pack(1)

#include <iostream>
#include <string>
#include <fstream>
#include <WinSock2.h>
using namespace std;

void err_quit(const char*);
void err_display(const char*);
int recvn(SOCKET, char*, int, int);

void File_Transfer();

const char* ServerIP{ "127.0.0.1" };
const u_short ServerPort{ 9000 };
const int BufferSize{ 500 };

int main()
{
	File_Transfer();
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

void File_Transfer()
{
	string file_name;
	cout << "������ ���� �� : ";
	cin >> file_name;

	ifstream in(file_name);
	if (!in)
	{
		cout << file_name << " ������ �����ϴ�." << endl;
		return;
	}

	// ���� �����͸� ������
	in.seekg(0, ios::end);
	// tellg( ) : ���� ũ�⸦ �˷���
	u_int file_size = in.tellg();
	// ���� �����͸� ó������
	in.seekg(0, ios::beg);

	int data;

	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

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

	// ���� �̸� ������
	// file_name.size() ���� const�� int ������ ��Ƽ� ������.
	int file_name_size = file_name.size();
	data = send(client_socket, (char*)&file_name_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		closesocket(client_socket);
		WSACleanup();
		return;
	}
	cout << "���� �̸�(��������) ���� Byte �� : " << data << "Byte" << endl;

	cout << "���� ������(�����̸�) ����(��������)" << endl;
	data = send(client_socket, file_name.c_str(), file_name_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		closesocket(client_socket);
		WSACleanup();
		return;
	}

	cout << "[ TCP Ŭ���̾�Ʈ ] - " << data << "Byte�� ���½��ϴ�." << endl;

	// ���� ���� ������
	char* file_data = new char[file_size];
	if (file_data == nullptr)
		return;

	for (int i = 0; i < file_size; ++i)
		in >> file_data[i];
	in.close();

	data = send(client_socket, (char*)&file_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		closesocket(client_socket);
		WSACleanup();
		return;
	}
	cout << "(��������)���� Byte �� : " << data << endl;

	cout << "���� ������(����) ����(��������)" << endl;
	data = send(client_socket, file_data, file_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("send( )");
		closesocket(client_socket);
		WSACleanup();
		return;
	}
	cout << "[ TCP Ŭ���̾�Ʈ ] - " << data << "Byte�� ���½��ϴ�." << endl;

	delete[] file_data;

	closesocket(client_socket);
	WSACleanup();
}