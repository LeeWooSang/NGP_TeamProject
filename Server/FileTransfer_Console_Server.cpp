#include "FileTransfer_Console_Server.h"

CFileTransfer_Console_Server::CFileTransfer_Console_Server()
{
}

CFileTransfer_Console_Server::~CFileTransfer_Console_Server()
{
	Release();
	closesocket(listen_socket);
}

void CFileTransfer_Console_Server::err_quit(const char* msg)
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

void CFileTransfer_Console_Server::err_display(const char* msg)
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

bool CFileTransfer_Console_Server::Socket_Init()
{
	// Socket( )
	// ��� ���� ����
	listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_socket == INVALID_SOCKET)
	{
		err_quit("socket( )");
		return false;
	}

	// bind( )
	SOCKADDR_IN server_addr;
	ZeroMemory(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(ServerPort);
	data = bind(listen_socket, (SOCKADDR*)&server_addr, sizeof(server_addr));
	if (data == SOCKET_ERROR)
	{
		err_quit("bind( )");
		return false;
	}

	// listen( )
	data = listen(listen_socket, SOMAXCONN);
	if (data == SOCKET_ERROR)
	{
		err_quit("listen( )");
		return false;
	}

	cout << "��� ���� ���� �Ϸ�" << endl;

	// Ŭ������ ����
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

		Packet_Recv(client_socket, client_addr, addrlen);
	}

	return true;
}

int CFileTransfer_Console_Server::recvn(SOCKET socket, char* buf, int len, int flag)
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

void CFileTransfer_Console_Server::Packet_Recv(SOCKET& client_socket, SOCKADDR_IN& client_addr, int& addrlen)
{

	// ������ Ŭ���̾�Ʈ ���� ���
	cout << endl << "[ TCP ���� ] Ŭ���̾�Ʈ ���� - IP : " << inet_ntoa(client_addr.sin_addr)
		<< ", ��Ʈ ��ȣ : " << ntohs(client_addr.sin_port) << endl;

	int file_name_size = 0;

	data = recvn(client_socket, (char*)&file_name_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	// �ι��ڰ� ���� ���� �̸����� +1�� ����
	packet.file_data = new char[file_name_size + 1];
	if (packet.file_data == nullptr)
		return;

	data = recvn(client_socket, packet.file_data, file_name_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	// �̸� ���� �ι��� �߰�
	packet.file_data[file_name_size] = '\0';

	cout << "�����̸� : " << packet.file_data << endl;

	// ���� ���� �����̸����� ���� ����
	ofstream out(packet.file_data, ios::binary);

	Release();

	size_t file_size = 0;

	// ������ �ޱ�( recvn( ) ) => ��������(���� ������ ������)
	data = recvn(client_socket, (char*)&file_size, sizeof(int), 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	packet.file_data = new char[BufferSize];
	if (packet.file_data == nullptr)
		return;

	// ���� �������� ũ��
	size_t recv_data_size = 0;
	// �� ���� ũ�� / ���� ������ �� �ϸ� ��� recvn( )�� ȣ�� �ؾ��ϴ��� �� �� �ִ�.
	size_t left_data_count = file_size / BufferSize;

	while (left_data_count > 0)
	{
		// ���� ������ �ޱ�( recvn( ) ) => ���� ����
		data = recvn(client_socket, packet.file_data, BufferSize, 0);
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

		out.write((const char*)packet.file_data, data * sizeof(char));

		--left_data_count;
	}
	Release();

	// ���� ������ �ޱ�
	packet.file_data = new char[file_size - recv_data_size ];
	data = recvn(client_socket, packet.file_data, file_size - recv_data_size, 0);
	if (data == SOCKET_ERROR)
	{
		err_display("recvn( )");
		return;
	}
	else if (data == 0)
		return;

	out.write((const char*)packet.file_data, (data) * sizeof(char));
	out.close();

	recv_data_size += data;

	cout << "[ TCP / " << inet_ntoa(client_addr.sin_addr) << ", " << ntohs(client_addr.sin_port)
		<< " ] - ���۷� : " << ((double)recv_data_size / file_size) * 100 << "%, ���� ������ ũ�� : " << recv_data_size << "Byte" << endl;
	cout << "���� �ٿ��� �������ϴ�." << endl;

	Release();

	// closesocket( ) - Ŭ�� ���� ���� �ݱ�
	closesocket(client_socket);
	cout << endl << "[ TCP ���� ] Ŭ���̾�Ʈ ���� - IP : " << inet_ntoa(client_addr.sin_addr)
		<< ", ��Ʈ ��ȣ : " << ntohs(client_addr.sin_port) << endl;
}

void CFileTransfer_Console_Server::Release()
{
	if (packet.file_data != nullptr)
	{
		delete[] packet.file_data;
		packet.file_data = nullptr;
	}
}