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
const u_short ServerPort{ 9000 };
const u_int BufferSize{ 5000 };
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

void File_Transfer()
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return;

	// Socket( )
	// ��� ���� ����
	SOCKET listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
		err_quit("socket( )");

	cout << "��� ���� ���� �Ϸ�" << endl;

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

	// ������ ��ſ� ����� Ŭ���̾�Ʈ ���� ���� ����
	SOCKET client_socket;
	SOCKADDR_IN client_addr;
	int addrlen;

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

		// ������ Ŭ���̾�Ʈ ���� ���
		cout << endl << "[ TCP ���� ] Ŭ���̾�Ʈ ���� - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", ��Ʈ ��ȣ : " << ntohs(client_addr.sin_port) << endl;

		int file_name_size = 0;

		data = recvn(client_socket, (char*)&file_name_size, sizeof(int), 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		// �ι��ڰ� ���� ���� �̸������� ����
		char* file_data = new char[file_name_size + 1];
		if (file_data == nullptr)
			return;

		data = recvn(client_socket, file_data, file_name_size, 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		file_data[data] = '\0';
		cout << file_data << endl;

		// ���� ���� �����̸����� ���� ����
		ofstream out(file_data);

		delete[] file_data;

		u_int file_size = 0;

		// ������ �ޱ�( recvn( ) ) => ��������(���� ������ ������)
		data = recvn(client_socket, (char*)&file_size, sizeof(int), 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		file_data = new char[BufferSize];
		if (file_data == nullptr)
			return;

		u_int recv_data_size = 0;

		// �� ���� ũ�� / ���� ������ �� �ϸ� ��� recvn( )�� ȣ�� �ؾ��ϴ��� �� �� �ִ�.
		int left_data_count = file_size / BufferSize;
		// BufferSize��ŭ �����͸� ����.
		
		while(left_data_count > 0)
		{
			// ���� ������ �ޱ�( recvn( ) ) => ���� ����
			data = recvn(client_socket, file_data, BufferSize, 0);
			if (data == SOCKET_ERROR)
			{
				err_display("recvn( )");
				break;
			}
			else if (data == 0)
				break;

			// ���� ������ ���
			recv_data_size += data;

			cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
				<< " ] - ���۷� : " << ((double)recv_data_size / file_size) * 100 << "%, ���� ������ ũ�� : " << recv_data_size << "Byte" << endl;

			for (int i = 0; i < data; ++i)
				out << file_data[i];

			--left_data_count;
		} 

		// ���� ������
		data = recvn(client_socket, file_data, file_size - recv_data_size, 0);
		if (data == SOCKET_ERROR)
		{
			err_display("recvn( )");
			break;
		}
		else if (data == 0)
			break;

		file_data[data] = '\0';

		for (int i = 0; i < data; ++i)
			out << file_data[i];
		out.close();

		recv_data_size += data;

		cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
			<< " ] - ���۷� : " << ((double)recv_data_size / file_size) * 100 << "%, ���� ������ ũ�� : " << recv_data_size << "Byte" << endl;
		cout << "���� ������ �������ϴ�." << endl;

		delete[] file_data;
		
		// closesocket( ) - Ŭ�� ���� ���� �ݱ�
		closesocket(client_socket);
		cout << endl << "[ TCP ���� ] Ŭ���̾�Ʈ ���� - IP : " << inet_ntoa(client_addr.sin_addr)
			<< ", ��Ʈ ��ȣ : " << ntohs(client_addr.sin_port) << endl;
	}

	// closesocket( ) - ��� ���� �ݱ�
	closesocket(listen_socket);

	// ���� ����
	WSACleanup();
}